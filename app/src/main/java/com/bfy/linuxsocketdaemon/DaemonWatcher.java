package com.bfy.linuxsocketdaemon;

/**
 * <pre>
 * @copyright  : Copyright ©2004-2018 版权所有　彩讯科技股份有限公司
 * @company    : 彩讯科技股份有限公司
 * @author     : OuyangJinfu
 * @e-mail     : ouyangjinfu@richinfo.cn
 * @createDate : 2018/7/10 0010
 * @modifyDate : 2018/7/10 0010
 * @version    : 1.0
 * @desc       :
 * </pre>
 */

public class DaemonWatcher {

    static {
        System.loadLibrary("daemon");
    }

    public native void createDaemonServer(String userid);

    public native void createDaemonClient();

}
