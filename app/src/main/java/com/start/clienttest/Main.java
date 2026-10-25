package com.start.clienttest;

import android.app.Activity;

import com.irene.algui.IMEHelper;

public class Main {
    public static Main instance;
    public Activity activity;

    public static void main(Activity act) {
        if (instance == null) instance = new Main();
        instance.initAndShow(act);
    }

    public void initAndShow(Activity ac) {
        this.activity = ac;
        if (!PermissionTool.isStoragePermissionGranted(activity)) {
            PermissionTool.storePermission(activity);
        }
        IMEHelper.init(activity);
    }
}