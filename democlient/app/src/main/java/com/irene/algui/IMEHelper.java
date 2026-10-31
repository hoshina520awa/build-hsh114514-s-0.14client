package com.irene.algui;

import android.app.Activity;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.graphics.PixelFormat;
import android.os.Handler;
import android.os.Looper;
import android.text.Editable;
import android.text.InputType;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;

import java.lang.ref.WeakReference;

public class IMEHelper {

    private static final String TAG = "IMEHelper";
    private static WeakReference<Activity> activityRef;
    private static InputMethodManager imm;
    private static InputMethodView inputView;
    private static WindowManager windowManager;
    private static volatile boolean isInputActive = false;
    private static volatile String currentText = "";
    private static InputTextCallback inputCallback;
    private static final Handler mainHandler = new Handler(Looper.getMainLooper());

    public interface InputTextCallback {
        void onInputTextChanged(String text);
    }

    public static void init(Activity activity) {
        if (activity == null) return;
        activityRef = new WeakReference<>(activity);
        imm = (InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE);
        windowManager = (WindowManager) activity.getSystemService(Context.WINDOW_SERVICE);
        inputView = new InputMethodView(activity);
        Log.i(TAG, "IMEHelper initialized");
    }

    public static void showKeyboard(String initialText) {
        currentText = initialText != null ? initialText : "";
        isInputActive = true;

        runOnMain(() -> {
            Activity activity = activityRef != null ? activityRef.get() : null;
            if (activity == null || inputView == null || imm == null) {
                Log.e(TAG, "showKeyboard failed: context/view is null");
                return;
            }

            inputView.setText(currentText);
            inputView.setFocusable(true);
            inputView.setFocusableInTouchMode(true);

            try {
                if (inputView.getParent() == null) {
                    WindowManager.LayoutParams params = new WindowManager.LayoutParams();
                    params.width = 1;
                    params.height = 1;
                    params.type = WindowManager.LayoutParams.TYPE_APPLICATION;
                    params.gravity = Gravity.START | Gravity.TOP;
                    params.x = 0;
                    params.y = 0;
                    params.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE
                            | WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN
                            | WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED;
                    params.format = PixelFormat.TRANSPARENT;

                    windowManager.addView(inputView, params);
                    Log.d(TAG, "inputView added to window");
                }
            } catch (Exception e) {
                Log.e(TAG, "Failed to add inputView to window", e);
            }

            boolean focusResult = inputView.requestFocus();

            imm.restartInput(inputView);

            inputView.postDelayed(() -> {
                try {
                    boolean result = imm.showSoftInput(inputView, InputMethodManager.SHOW_FORCED);
                    if (!result) {
                        imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, 0);
                    }
                } catch (Exception e) {
                    Log.e(TAG, "showSoftInput exception", e);
                }
            }, 100);
        });
    }

    public static void hideKeyboard() {
        runOnMain(() -> {
            if (inputView != null && imm != null) {
                try {
                    imm.hideSoftInputFromWindow(inputView.getWindowToken(), 0);
                } catch (Exception e) {
                    Log.e(TAG, "hideSoftInput exception", e);
                }
                inputView.clearFocus();

                if (inputView.getParent() != null && windowManager != null) {
                    try {
                        windowManager.removeView(inputView);
                        Log.d(TAG, "inputView removed from window");
                    } catch (Exception e) {
                        Log.e(TAG, "removeView exception", e);
                    }
                }
            }
            isInputActive = false;
            Log.d(TAG, "hideKeyboard");
        });
    }

    private static int dipToPx(Context context, float dip) {
        float density = context.getResources().getDisplayMetrics().density;
        return (int) (dip * density + 0.5f);
    }

    private static void copyToClipboard(Context context) {
        android.content.ClipboardManager clipboard =
                (android.content.ClipboardManager) context.getSystemService(Context.CLIPBOARD_SERVICE);
        android.content.ClipData clip = android.content.ClipData.newPlainText("text", currentText);
        clipboard.setPrimaryClip(clip);
    }

    private static void cutToClipboard(Context context) {
        copyToClipboard(context);
        currentText = "";
        if (inputCallback != null) {
            inputCallback.onInputTextChanged(currentText);
        }
    }

    private static void pasteFromClipboard(Context context) {
        android.content.ClipboardManager clipboard =
                (android.content.ClipboardManager) context.getSystemService(Context.CLIPBOARD_SERVICE);
        if (clipboard != null && clipboard.hasPrimaryClip()) {
            android.content.ClipData clip = clipboard.getPrimaryClip();
            if (clip != null && clip.getItemCount() > 0) {
                CharSequence pasteText = clip.getItemAt(0).getText();
                if (pasteText != null) {
                    currentText = currentText + pasteText.toString();
                    if (inputCallback != null) {
                        inputCallback.onInputTextChanged(currentText);
                    }
                }
            }
        }
    }

    public static String getInputText() {
        return currentText;
    }

    public static boolean isKeyboardVisible() {
        return isInputActive;
    }

    public static void setInputCallback(InputTextCallback callback) {
        inputCallback = callback;
    }

    public static void copyToClipboard(String text) {
        runOnMain(() -> {
            Activity activity = activityRef != null ? activityRef.get() : null;
            if (activity == null) return;
            ClipboardManager clipboard = (ClipboardManager) activity.getSystemService(Context.CLIPBOARD_SERVICE);
            if (clipboard != null) {
                ClipData clip = ClipData.newPlainText("text", text);
                clipboard.setPrimaryClip(clip);
            }
        });
    }

    public static String pasteFromClipboard() {
        Activity activity = activityRef != null ? activityRef.get() : null;
        if (activity == null) return "";
        ClipboardManager clipboard = (ClipboardManager) activity.getSystemService(Context.CLIPBOARD_SERVICE);
        if (clipboard != null && clipboard.hasPrimaryClip()) {
            ClipData clip = clipboard.getPrimaryClip();
            if (clip != null && clip.getItemCount() > 0) {
                CharSequence text = clip.getItemAt(0).getText();
                return text != null ? text.toString() : "";
            }
        }
        return "";
    }

    private static void runOnMain(Runnable action) {
        if (Looper.getMainLooper() == Looper.myLooper()) {
            action.run();
        } else {
            mainHandler.post(action);
        }
    }

    static class InputMethodView extends View {
        private String text = "";

        public InputMethodView(Context context) {
            super(context);
            setFocusable(true);
            setFocusableInTouchMode(true);
        }

        public void setText(String text) {
            this.text = text != null ? text : "";
        }

        @Override
        public boolean onCheckIsTextEditor() {
            return true;
        }

        @Override
        public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
            outAttrs.imeOptions = EditorInfo.IME_ACTION_DONE | EditorInfo.IME_FLAG_NO_FULLSCREEN;
            outAttrs.inputType = InputType.TYPE_CLASS_TEXT;

            outAttrs.initialSelStart = currentText.length();
            outAttrs.initialSelEnd = currentText.length();

            return new BaseInputConnection(this, true) {
                @Override
                public Editable getEditable() {
                    return new android.text.SpannableStringBuilder(currentText);
                }

                @Override
                public boolean commitText(CharSequence text, int newCursorPosition) {
                    String newText = text.toString();
                    currentText = currentText + newText;
                    if (inputCallback != null) {
                        inputCallback.onInputTextChanged(currentText);
                    }
                    return true;
                }

                @Override
                public boolean setComposingText(CharSequence text, int newCursorPosition) {
                    return super.setComposingText(text, newCursorPosition);
                }

                @Override
                public boolean deleteSurroundingText(int beforeLength, int afterLength) {
                    boolean deleted = false;
                    if (beforeLength > 0 && currentText.length() >= beforeLength) {
                        int startPos = currentText.length() - beforeLength;
                        currentText = currentText.substring(0, startPos);
                        deleted = true;
                    }

                    if (afterLength > 0 && currentText.length() > afterLength) {
                        currentText = currentText.substring(0, currentText.length() - afterLength);
                        deleted = true;
                    }

                    if (deleted && inputCallback != null) {
                        inputCallback.onInputTextChanged(currentText);
                    }

                    return true;
                }

                @Override
                public boolean sendKeyEvent(KeyEvent event) {
                    if (event.getAction() == KeyEvent.ACTION_DOWN) {
                        if (event.getKeyCode() == KeyEvent.KEYCODE_DEL) {
                            if (!currentText.isEmpty()) {
                                currentText = currentText.substring(0, currentText.length() - 1);
                                if (inputCallback != null) {
                                    inputCallback.onInputTextChanged(currentText);
                                }
                            }
                            return true;
                        }
                    }
                    return super.sendKeyEvent(event);
                }
            };
        }
    }
}