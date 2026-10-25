package com.mojang.minecraftpe;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.app.AlertDialog;
import android.app.NativeActivity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Rect;
import android.media.AudioManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.SystemClock;
import android.os.Vibrator;
import android.preference.PreferenceManager;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.mojang.android.StringValue;
import com.mojang.minecraftpe.input.InputDeviceManager;
import com.mojang.minecraftpe.platforms.Platform;
import com.start.clienttest.Main;

import org.fmod.FMOD;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.Objects;
import java.util.UUID;

public class MainActivity extends NativeActivity implements View.OnKeyListener {
    static final String TAG = "Start Java";
    private Bundle data;
    private InputDeviceManager deviceManager;
    HeadsetConnectionReceiver headsetConnectionReceiver;
    private AlertDialog mDialog;
    PopupWindow mHiddenTextInputDialog;
    Platform platform;
    TextInputProxyEditTextbox textInputWidget;
    public static MainActivity mInstance = null;
    private static boolean _isPowerVr = false;
    public static int RESULT_PICK_IMAGE = 1;
    public static int RESULT_GOOGLEPLAY_PURCHASE = 2;
    public int virtualKeyboardHeight = 0;
    private boolean _isTouchscreen = true;
    private boolean _fromOnCreate = false;
    private int _viewDistance = 2;
    private long mFileDialogCallback = 0;
    List<ActivityListener> mActivityListeners = new ArrayList<>();
    private int _userInputStatus = -1;
    private String[] _userInputText = null;
    private ArrayList<StringValue> _userInputValues = new ArrayList<>();
    private final DateFormat DateFormat = new SimpleDateFormat();
    private long mCallback = 0;
    public String preloadingHTML = "";

    public class AnonymousClass3 implements TextInputProxyEditTextbox.MCPEKeyWatcher {
        private final MainActivity this$0;

        public AnonymousClass3(MainActivity mainActivity) {
            this.this$0 = mainActivity;
        }

        @Override
        public void onBackKeyPressed() {
            this.this$0.runOnUiThread(() -> {
                Log.w("mcpe - keyboard", "textInputWidget.onBackPressed");
                this.this$0.nativeBackPressed();
            });
        }

        @Override
        public void onDeleteKeyPressed() {
            this.this$0.runOnUiThread(this.this$0::nativeBackSpacePressed);
        }
    }

    private class HeadsetConnectionReceiver extends BroadcastReceiver {
        private HeadsetConnectionReceiver() {
        }

        @Override
        public void onReceive(Context context, Intent intent) {
            if (Objects.equals(intent.getAction(), "android.intent.action.HEADSET_PLUG")) {
                switch (intent.getIntExtra("state", -1)) {
                    case 0:
                        Log.d("MCPE", "Headset unplugged");
                        MainActivity.this.nativeSetHeadphonesConnected(false);
                        break;
                    case 1:
                        Log.d("MCPE", "Headset plugged in");
                        MainActivity.this.nativeSetHeadphonesConnected(true);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    static {
        System.loadLibrary("gnustl_shared");
        try {
            System.loadLibrary("Mod");
        } catch (UnsatisfiedLinkError e) {
            Log.d("Start", "Lib not found!!");
        }
        System.loadLibrary("fmod");
        System.loadLibrary("minecraftpe");
    }

    public static boolean isPowerVR() {
        return _isPowerVr;
    }

    public void onDialogCanceled() {
        this._userInputStatus = 0;
    }

    private void processIntent(Intent intent) {
        Uri data;
        if (intent == null) {
            return;
        }
        String stringExtra = intent.getStringExtra("intent_cmd");
        if (stringExtra == null || stringExtra.length() <= 0) {
            String action = intent.getAction();
            String type = intent.getType();
            if ("android.intent.action.VIEW".equals(action) && "text/plain".equalsIgnoreCase(type) && (data = intent.getData()) != null) {
                String host = data.getHost();
                String query = data.getQuery();
                if (host == null && query == null) {
                    return;
                }
                nativeProcessIntentUriQuery(host, query);
                return;
            }
            return;
        }
        try {
            JSONObject jSONObject = new JSONObject(stringExtra);
            String string = jSONObject.getString("Command");
            if (string.equals("keyboardResult")) {
                nativeSetTextboxText(jSONObject.getString("Text"));
            } else if (!string.equals("fileDialogResult") || this.mFileDialogCallback == 0) {
            } else {
                if (jSONObject.getString("Result").equals("Ok")) {
                    nativeOnPickImageSuccess(this.mFileDialogCallback, jSONObject.getString("Path"));
                } else {
                    nativeOnPickImageCanceled(this.mFileDialogCallback);
                }
                this.mFileDialogCallback = 0L;
            }
        } catch (JSONException e) {
            Log.d("MCPE", "JSONObject exception:" + e.toString());
        }
    }

    public static void saveScreenshot(String str, int i, int i2, int[] iArr) {
        Bitmap createBitmap = Bitmap.createBitmap(iArr, i, i2, Bitmap.Config.ARGB_8888);
        try {
            FileOutputStream fileOutputStream = new FileOutputStream(str);
            createBitmap.compress(Bitmap.CompressFormat.JPEG, 85, fileOutputStream);
            try {
                fileOutputStream.flush();
                fileOutputStream.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        } catch (FileNotFoundException e3) {
            System.err.println("Couldn't create file: " + str);
            e3.printStackTrace();
        }
    }

    public void addListener(ActivityListener activityListener) {
        this.mActivityListeners.add(activityListener);
    }

    public void buyGame() {
    }

    public int checkLicense() {
        return 0;
    }

    public void clearLoginInformation() {
        SharedPreferences.Editor edit = PreferenceManager.getDefaultSharedPreferences(this).edit();
        edit.remove("accessToken");
        edit.remove("clientId");
        edit.remove("profileId");
        edit.remove("profileName");
        edit.commit();
    }

    public String createUUID() {
        return UUID.randomUUID().toString().replaceAll("-", "");
    }

    @Override // android.app.Activity, android.view.Window.Callback
    public boolean dispatchKeyEvent(KeyEvent keyEvent) {
        if (keyEvent.getAction() == KeyEvent.ACTION_MULTIPLE
                && keyEvent.getKeyCode() == KeyEvent.KEYCODE_UNKNOWN) {
            try {
                nativeTypeCharacter(keyEvent.getCharacters());
                return true;
            } catch (UnsatisfiedLinkError e) {
                // Do nothing
            }
        }
        return super.dispatchKeyEvent(keyEvent);
    }

    public void displayDialog(int i) {
    }

    public String getAccessToken() {
        return PreferenceManager.getDefaultSharedPreferences(this).getString("accessToken", "");
    }

    public int getAndroidVersion() {
        return Build.VERSION.SDK_INT;
    }

    public String[] getBroadcastAddresses() {
        Log.i(TAG, "get broadcast addresses");
        return new String[]{"255.255.255.255"};
    }

    public String getClientId() {
        return PreferenceManager.getDefaultSharedPreferences(this).getString("clientId", "");
    }

    public String getDateString(int time) {
        return DateFormat.getDateInstance(DateFormat.SHORT, Locale.US).format(new Date(((long) time) * 1000));
    }

    public String getDeviceID() {
        SharedPreferences defaultSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        String string = defaultSharedPreferences.getString("snooperId", "");
        if (string.isEmpty()) {
            string = UUID.randomUUID().toString().replaceAll("-", "");
            SharedPreferences.Editor edit = defaultSharedPreferences.edit();
            edit.putString("snooperId", string);
            edit.apply();
        }
        return string;
    }

    public String getDeviceModel() {
        return HardwareInformation.getDeviceModelName();
    }

    public String getExternalStoragePath() {
        return Environment.getExternalStorageDirectory().getAbsolutePath();
    }

    public byte[] getFileDataBytes(String str) {
        if (str == null || str.isEmpty()) {
            return null;
        }

        try (InputStream open = getApplicationContext().getAssets().open(str);
             ByteArrayOutputStream byteArrayOutputStream2 = new ByteArrayOutputStream()) {
            byte[] bArr2 = new byte[65536];
            int read2;
            while ((read2 = open.read(bArr2)) != -1) {
                byteArrayOutputStream2.write(bArr2, 0, read2);
            }
            return byteArrayOutputStream2.toByteArray();
        } catch (Exception e) {
            Log.e(TAG, "getFileDataBytes exception", e);
            //e.printStackTrace();
        }

        return null;
    }

    public int[] getImageData(String str, boolean z) {
        if (str == null || str.isEmpty()) {
            return null;
        }
        Bitmap bitmap = null;

        if (z) {
            try (InputStream open = getApplicationContext().getAssets().open(str)) {
                bitmap = BitmapFactory.decodeStream(open);
            } catch (Exception e) {
                Log.e(TAG, "getFileDataBytes exception", e);
            }
        } else {
            File file = new File(str);
            if (file.exists() && file.isFile()) {
                bitmap = BitmapFactory.decodeFile(str);
            }
        }

        if (bitmap == null) {
            return null;
        }

        int width = bitmap.getWidth();
        int height = bitmap.getHeight();

        int[] iArr = new int[(width * height) + 2];
        iArr[0] = width;
        iArr[1] = height;

        bitmap.getPixels(iArr, 2, width, 0, 0, width, height);

        bitmap.recycle();

        return iArr;
    }

    public int getKeyFromKeyCode(int keyCode, int metaState, int deviceId) {
        KeyCharacterMap characterMap = KeyCharacterMap.load(deviceId);
        return characterMap.get(keyCode, metaState);
    }

    public float getKeyboardHeight() {
        return this.virtualKeyboardHeight;
    }

    public String getLocale() {
        return HardwareInformation.getLocale();
    }

    public float getPixelsPerMillimeter() {
        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        return ((displayMetrics.xdpi + displayMetrics.ydpi) * 0.5f) / 25.4f;
    }

    public String getPlatformStringVar(int i) {
        return i == 0 ? Build.MODEL : null;
    }

    public String getProfileId() {
        return PreferenceManager.getDefaultSharedPreferences(this).getString("profileId", "");
    }

    public String getProfileName() {
        return PreferenceManager.getDefaultSharedPreferences(this).getString("profileName", "");
    }

    public int getScreenHeight() {
        Display defaultDisplay = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        int min = Math.min(defaultDisplay.getWidth(), defaultDisplay.getHeight());
        return min;
    }

    public int getScreenWidth() {
        Display defaultDisplay = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        int max = Math.max(defaultDisplay.getWidth(), defaultDisplay.getHeight());
        return max;
    }

    public long getTotalMemory() {
        ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
        activityManager.getMemoryInfo(memoryInfo);
        return memoryInfo.availMem;
    }

    public int getUserInputStatus() {
        return this._userInputStatus;
    }

    public String[] getUserInputString() {
        return this._userInputText;
    }

    public boolean hasBuyButtonWhenInvalidLicense() {
        return true;
    }

    boolean hasHardwareChanged() {
        SharedPreferences defaultSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        String string = defaultSharedPreferences.getString("lastAndroidVersion", "");
        boolean z = string.isEmpty() || !string.equals(Build.VERSION.RELEASE);
        if (z) {
            SharedPreferences.Editor edit = defaultSharedPreferences.edit();
            edit.putString("lastAndroidVersion", Build.VERSION.RELEASE);
            edit.apply();
        }
        return z;
    }

    public void hideKeyboard() {
        runOnUiThread(() -> {
            if (MainActivity.this.mHiddenTextInputDialog != null) {
                MainActivity.this.mHiddenTextInputDialog.dismiss();
                MainActivity.this.mHiddenTextInputDialog = null;
            }
        });
    }

    public void initiateUserInput(int i) {
        this._userInputText = null;
        this._userInputStatus = -1;
    }

    protected boolean isDemo() {
        return false;
    }

    public boolean isFirstSnooperStart() {
        return PreferenceManager.getDefaultSharedPreferences(this).getString("snooperId", "").isEmpty();
    }

    public boolean isNetworkEnabled(boolean z) {
        return true;
    }

    boolean isTablet() {
        return (getResources().getConfiguration().screenLayout & 15) == 4;
    }

    public void launchUri(String str) {
        startActivity(new Intent("android.intent.action.VIEW", Uri.parse(str)));
    }

    native void nativeBackPressed();

    native void nativeBackSpacePressed();

    native boolean nativeKeyHandler(int i, int i2);

    native void nativeOnDestroy();

    native void nativeOnPickImageCanceled(long j);

    native void nativeOnPickImageSuccess(long j, String str);

    native void nativeProcessIntentUriQuery(String str, String str2);

    native void nativeRegisterThis();

    native void nativeReturnKeyPressed();

    native void nativeSetHeadphonesConnected(boolean z);

    native void nativeSetTextboxText(String str);

    native void nativeStopThis();

    native void nativeSuspend();

    native void nativeTypeCharacter(String str);

    native void nativeUnregisterThis();

    @SuppressLint("Range")
    protected void onActivityResult(int i, int i2, Intent intent) {
        super.onActivityResult(i, i2, intent);
        for (ActivityListener activityListener : this.mActivityListeners) {
            activityListener.onActivityResult(i, i2, intent);
        }
        if (i == RESULT_PICK_IMAGE) {
            if (i2 != -1 || intent == null) {
                if (this.mCallback != 0) {
                    nativeOnPickImageCanceled(this.mCallback);
                    this.mCallback = 0L;
                    return;
                }
                return;
            }
            String[] strArr = {"_data"};
            Cursor query = getContentResolver().query(Objects.requireNonNull(intent.getData()), strArr, null, null, null);
            query.moveToFirst();
            nativeOnPickImageSuccess(this.mCallback, query.getString(query.getColumnIndex(strArr[0])));
            this.mCallback = 0L;
            query.close();
        }

    }

    @SuppressLint("GestureBackNavigation")
    @Override
    public void onBackPressed() {
        nativeBackPressed();
    }

    @Override // android.app.NativeActivity, android.app.Activity
    public void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        Main.main(this);
        this.platform = Platform.createPlatform(true);
        setVolumeControlStream(3);
        nativeRegisterThis();
        FMOD.init(this);
        this.deviceManager = InputDeviceManager.create(this);
        this.platform.onAppStart(getWindow().getDecorView());
        this.headsetConnectionReceiver = new HeadsetConnectionReceiver();
        nativeSetHeadphonesConnected(((AudioManager) getSystemService(Context.AUDIO_SERVICE)).isWiredHeadsetOn());
        mInstance = this;
        this._fromOnCreate = true;
    }

    @Override // android.app.NativeActivity, android.app.Activity
    protected void onDestroy() {
        mInstance = null;
        System.out.println("onDestroy");
        FMOD.close();
        for (ActivityListener activityListener : this.mActivityListeners) {
            activityListener.onDestroy();
        }
        nativeUnregisterThis();
        nativeOnDestroy();
        super.onDestroy();
        System.exit(0);
    }

    @Override // android.view.View.OnKeyListener
    public boolean onKey(View view, int i, KeyEvent keyEvent) {
        return false;
    }

    @Override // android.app.Activity, android.view.KeyEvent.Callback
    public boolean onKeyDown(int i, KeyEvent keyEvent) {
        return super.onKeyDown(i, keyEvent);
    }

    @Override // android.app.Activity, android.view.KeyEvent.Callback
    public boolean onKeyMultiple(int i, int i2, KeyEvent keyEvent) {
        return super.onKeyMultiple(i, i2, keyEvent);
    }

    @Override // android.app.Activity, android.view.KeyEvent.Callback
    public boolean onKeyUp(int i, KeyEvent keyEvent) {
        if (i == 25 || i == 24) {
            this.platform.onVolumePressed();
        }
        return super.onKeyUp(i, keyEvent);
    }

    @Override // android.app.Activity
    public void onNewIntent(Intent intent) {
        processIntent(intent);
    }

    @Override // android.app.NativeActivity, android.app.Activity
    protected void onPause() {
        nativeSuspend();
        super.onPause();
    }

    @Override // android.app.NativeActivity, android.app.Activity
    protected void onResume() {
        super.onResume();
        registerReceiver(this.headsetConnectionReceiver, new IntentFilter("android.intent.action.HEADSET_PLUG"));
        if (this.mHiddenTextInputDialog != null) {
            String obj = this.textInputWidget.getText().toString();
            int i = this.textInputWidget.allowedLength;
            boolean z = this.textInputWidget.limitInput;
            this.textInputWidget.getInputType();
            this.mHiddenTextInputDialog.dismiss();
            this.mHiddenTextInputDialog = null;
            showKeyboard(obj, i, z, z);
        }
    }

    @Override // android.app.NativeActivity, android.app.Activity
    @SuppressLint({"DefaultLocale"})
    protected void onStart() {
        super.onStart();
        this.deviceManager.register();
        if (this._fromOnCreate) {
            this._fromOnCreate = false;
            processIntent(getIntent());
        }
    }

    @Override // android.app.NativeActivity, android.app.Activity
    protected void onStop() {
        nativeStopThis();
        super.onStop();
        this.deviceManager.unregister();
    }

    @Override // android.app.NativeActivity, android.app.Activity, android.view.Window.Callback
    public void onWindowFocusChanged(boolean z) {
        super.onWindowFocusChanged(z);
        this.platform.onViewFocusChanged(z);
        if (z){
//            View decorView = getWindow().getDecorView();
//            decorView.postDelayed(() -> {
//                try {
//                    if (hasWindowFocus()) {
//                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
//                            decorView.requestPointerCapture();
//                        }
//                    }
//                } catch (Exception e) {
//                    e.printStackTrace();
//                }
//            }, 300);
        }

    }

    void pickImage(long callbackAddress) {
        this.mCallback = callbackAddress;
        Intent picker = new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        startActivityForResult(picker, RESULT_PICK_IMAGE);
    }

    public void postScreenshotToFacebook(String name, int firstInt, int secondInt, int[] thatArray) {
    }

    public void quit() {
        finish();
    }

    public void removeListener(ActivityListener activityListener) {
        this.mActivityListeners.remove(activityListener);
    }

    void setFileDialogCallback(long j) {
        this.mFileDialogCallback = j;
    }

    public void setIsPowerVR(boolean z) {
        _isPowerVr = z;
    }

    public void setLoginInformation(String str, String str2, String str3, String str4) {
        SharedPreferences.Editor edit = PreferenceManager.getDefaultSharedPreferences(this).edit();
        edit.putString("accessToken", str);
        edit.putString("clientId", str2);
        edit.putString("profileId", str3);
        edit.putString("profileName", str4);
        edit.apply();
    }

    public void setRefreshToken(String str) {
        SharedPreferences.Editor edit = PreferenceManager.getDefaultSharedPreferences(this).edit();
        edit.putString("refreshToken", str);
        edit.apply();
    }

    public void setSession(String str) {
        SharedPreferences.Editor edit = PreferenceManager.getDefaultSharedPreferences(this).edit();
        edit.putString("sessionID", str);
        edit.apply();
    }

    public void setupKeyboardViews(String str, int i, boolean z, boolean z2) {
        this.textInputWidget = new TextInputProxyEditTextbox(this, i, z);
        this.textInputWidget.setFocusable(true);
        this.textInputWidget.setFocusableInTouchMode(true);
        this.textInputWidget.setInputType(131072);
        this.textInputWidget.setImeOptions(268435461);
        this.textInputWidget.setText(str);

        if (z2) {
            this.textInputWidget.setInputType(2);
        }

        this.textInputWidget.setOnEditorActionListener((TextView textView, int i2, KeyEvent keyEvent) -> {
            Log.w("mcpe - keyboard", "onEditorAction: " + i2);
            if (i2 == 5) {
                this.nativeReturnKeyPressed();
                return true;
            } else if (i2 == 7) {
                this.nativeBackPressed();
                return true;
            }
            return false;
        });

        this.textInputWidget.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence charSequence, int start, int before, int count) {
            }

            @Override
            public void afterTextChanged(Editable editable) {
                MainActivity.this.nativeSetTextboxText(editable.toString());
            }
        });

        this.textInputWidget.setOnMCPEKeyWatcher(new AnonymousClass3(this));

        this.mHiddenTextInputDialog = new PopupWindow(this);
        this.mHiddenTextInputDialog.setInputMethodMode(PopupWindow.INPUT_METHOD_NEEDED);
        this.mHiddenTextInputDialog.setWidth(320);
        this.mHiddenTextInputDialog.setHeight(50);
        this.mHiddenTextInputDialog.setWindowLayoutMode(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        this.mHiddenTextInputDialog.setClippingEnabled(false);

        LinearLayout linearLayout = new LinearLayout(this);
        linearLayout.setPadding(-5, -5, -5, -5);
        linearLayout.setOrientation(LinearLayout.VERTICAL);

        View linearLayout2 = new LinearLayout(this);
        ViewGroup.MarginLayoutParams marginLayoutParams = new ViewGroup.MarginLayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        marginLayoutParams.setMargins(0, 0, 0, 0);

        linearLayout.addView(this.textInputWidget, marginLayoutParams);
        this.mHiddenTextInputDialog.setContentView(linearLayout);

        setContentView(linearLayout2, marginLayoutParams);

        this.mHiddenTextInputDialog.setOutsideTouchable(true);
        this.mHiddenTextInputDialog.setTouchInterceptor((View view, MotionEvent motionEvent) -> {
            this.nativeBackPressed();
            return false;
        });

        this.mHiddenTextInputDialog.showAtLocation(linearLayout2, 0, -50000, 0);
        this.mHiddenTextInputDialog.setFocusable(true);
        this.mHiddenTextInputDialog.update();
        this.mHiddenTextInputDialog.dismiss();

        this.mHiddenTextInputDialog.showAtLocation(linearLayout2, 0, -50000, 0);
        this.mHiddenTextInputDialog.setFocusable(true);
        this.mHiddenTextInputDialog.update();

        this.textInputWidget.postDelayed(() -> {
            long uptimeMillis = SystemClock.uptimeMillis();

            MotionEvent obtain = MotionEvent.obtain(uptimeMillis, uptimeMillis, MotionEvent.ACTION_DOWN, 0.0f, 0.0f, 0);
            this.textInputWidget.dispatchTouchEvent(obtain);
            obtain.recycle();

            MotionEvent obtain2 = MotionEvent.obtain(uptimeMillis, uptimeMillis, MotionEvent.ACTION_UP, 0.0f, 0.0f, 0);
            this.textInputWidget.dispatchTouchEvent(obtain2);
            obtain2.recycle();

            this.textInputWidget.setSelection(this.textInputWidget.length());
        }, 200L);

        View rootView = findViewById(android.R.id.content).getRootView(); // 16908290 是 android.R.id.content
        rootView.getViewTreeObserver().addOnGlobalLayoutListener(() -> {
            Rect rect = new Rect();
            rootView.getWindowVisibleDisplayFrame(rect);
            MainActivity.this.virtualKeyboardHeight = rootView.getRootView().getHeight() - rect.height();
        });
    }

    public void showKeyboard(final String str, final int i, final boolean z, final boolean z2) {
        runOnUiThread(() -> MainActivity.this.setupKeyboardViews(str, i, z, z2));
    }

    public void statsTrackEvent(String str, String str2) {
    }

    public void statsUpdateUserData(String str, String str2) {
    }

    public void tick() {
    }

    public void updateTextboxText(final String str) {
        runOnUiThread(() -> {
            if (MainActivity.this.textInputWidget != null) {
                MainActivity.this.textInputWidget.setText(str);
                MainActivity.this.textInputWidget.setSelection(MainActivity.this.textInputWidget.length());
            }
        });
    }

    public void vibrate(int i) {
        ((Vibrator) getSystemService(Context.VIBRATOR_SERVICE)).vibrate(i);
    }
}