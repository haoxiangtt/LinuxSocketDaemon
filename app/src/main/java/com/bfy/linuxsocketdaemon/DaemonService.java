package com.bfy.linuxsocketdaemon;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;

/**
 * <pre>
 * @copyright  : Copyright ©2004-2018 版权所有　彩讯科技股份有限公司
 * @company    : 彩讯科技股份有限公司
 * @author     : OuyangJinfu
 * @e-mail     : ouyangjinfu@richinfo.cn
 * @createDate : 2018/7/11 0011
 * @modifyDate : 2018/7/11 0011
 * @version    : 1.0
 * @desc       :
 * </pre>
 */

public class DaemonService extends Service {

    DaemonWatcher dw;

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        dw = new DaemonWatcher();
        dw.createDaemonServer("com.bfy.linuxsocketdaemon");
        new Thread(new Runnable() {
            @Override
            public void run() {
                dw.createDaemonClient();
            }
        }).start();
    }


}
