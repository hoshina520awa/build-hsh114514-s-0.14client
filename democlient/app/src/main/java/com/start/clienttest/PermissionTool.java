package com.start.clienttest;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.Settings;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

public class PermissionTool {

    private static final int STORAGE_PERMISSION_REQUEST_CODE = 102;
    private static final int AUDIO_PERMISSION_REQUEST_CODE = 103;

    public static void storePermission(Activity activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                try {
                    Intent intent = new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
                    intent.setData(Uri.parse("package:" + activity.getPackageName()));
                    activity.startActivity(intent);
                } catch (Exception e) {
                    Intent intent = new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
                    activity.startActivity(intent);
                }
            }
        } else {
            ActivityCompat.requestPermissions(
                    activity,
                    new String[]{
                            Manifest.permission.READ_EXTERNAL_STORAGE,
                            Manifest.permission.WRITE_EXTERNAL_STORAGE
                    },
                    STORAGE_PERMISSION_REQUEST_CODE
            );
        }
    }

    public static void floatingWindowPermission(Context context) {
        if (context == null) {
            return;
        }

        if (!Settings.canDrawOverlays(context)) {
            Intent intent = new Intent(
                    Settings.ACTION_MANAGE_OVERLAY_PERMISSION,
                    Uri.parse("package:" + context.getPackageName())
            );
            context.startActivity(intent);
        }
    }

    public static void requestAudioPermission(Activity activity) {
        if (activity.checkSelfPermission(Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
            activity.requestPermissions(new String[]{Manifest.permission.RECORD_AUDIO}, AUDIO_PERMISSION_REQUEST_CODE);
        }
    }

    public static boolean requestAllPermissions(Context context, Activity activity) {
        storePermission(activity);
        floatingWindowPermission(context);
        return true;
    }

    public static boolean isStoragePermissionGranted(Context context) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            return Environment.isExternalStorageManager();
        }
        return ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.READ_EXTERNAL_STORAGE
        ) == PackageManager.PERMISSION_GRANTED
                && ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.WRITE_EXTERNAL_STORAGE
        ) == PackageManager.PERMISSION_GRANTED;
    }

    public static boolean isFloatingWindowPermissionGranted(Context context) {
        if (context == null) {
            return false;
        }
        return Settings.canDrawOverlays(context);
    }

    public static boolean isAudioPermissionGranted(Context context) {
        return context.checkSelfPermission(Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED;
    }


    @SuppressLint("InlinedApi")
    public static boolean checkNotificationPermission(Context context) {
        return ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.POST_NOTIFICATIONS
        ) == PackageManager.PERMISSION_GRANTED;
    }


    public static boolean areAllPermissionsGranted(Context context) {
        return isStoragePermissionGranted(context) &&
                isFloatingWindowPermissionGranted(context) &&
                checkNotificationPermission(context);
    }
}
