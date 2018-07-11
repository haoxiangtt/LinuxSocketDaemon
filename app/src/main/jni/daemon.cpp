// Created by ouyangjinfu on 2018/7/10 0010.
//
#include "daemon.h" //这里注意， 如果刚才没有改文件名，那么这里就不是daemon.h了

char *USER_ID;
pid_t fpid;
const char *SOCKET_PATH = "/data/data/com.bfy.linuxsocketdaemon/deamon.sock";
const int CLI_NUM = 3;
const int BUFF_SIZE = 256;

void Java_com_bfy_linuxsocketdaemon_DaemonWatcher_createDaemonServer
  (JNIEnv *evn, jobject cls, jstring userid){
    USER_ID = jstringToChar(evn, userid);
    LOGD(">>>>>hello world for jni<<<<<<userid=%s", USER_ID);
    fpid=fork();
    if (fpid < 0){
        LOGE(">>>>>error fock()<<<<<<");
    } else if (fpid == 0) {
        LOGD(">>>>I am a child process, my process id is %d/n", getpid());
        createServerSocket();
    } else {
        LOGD(">>>>I am a parent process, my process id is %d/n", getpid());
    }
  }
void JNICALL Java_com_bfy_linuxsocketdaemon_DaemonWatcher_createDaemonClient
  (JNIEnv *evn, jobject cls) {
    if (fpid <= 0) {
        LOGE(">>>>>>fpid is %d", fpid);
        return;
    }
    int sock_fd;
    struct sockaddr_un client_addr;
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        LOGE(">>>>create client socket error<<<<");
        return;
    }
    LOGD(">>>>Client socket ok!<<<<<");

    /*unlink(SOCKET_PATH);
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, SOCKET_PATH);
    int bfd = bind(sock_fd, (struct sockaddr *)&client_addr, sizeof(client_addr));
    if (bfd == -1) {
        LOGE(">>>>client socket bind failed<<<<");
        return;
    }
    LOGD(">>>client socket bind ok!<<<<");*/

    while(1) {
        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sun_family = AF_UNIX;
        strcpy(client_addr.sun_path, SOCKET_PATH);
//        int len = offsetof(struct sockaddr_un, sun_path) + strlen(SOCKET_PATH);
        int cfd = connect(sock_fd, (struct sockaddr *) &client_addr, sizeof(client_addr));
        if (cfd < 0) {
            LOGE(">>>client socket connect failed<<<<");
            sleep(3);
            continue;
        }
        break;
    }
    LOGD(">>>client socket connect ok!<<<<");
  }


void createServerSocket(){
    //unsigned short SERVER_PORT = 0x8888;
    int server_sock_fd;
    struct sockaddr_un server_addr;
    server_sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock_fd < 0){
        LOGE(">>>>>create server socket error<<<<<");
        return;
    }
    LOGD(">>>>>Server socket ok!<<<<<");
    unlink(SOCKET_PATH);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family  = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path)-1);
    int bfd = bind(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(bfd == -1) {
        LOGE(">>>>>server socket bind failed<<<<");
        return;
    }
    LOGD(">>>server socket bind ok<<<<<");
    if (listen(server_sock_fd, 5) == -1) {
        LOGE(">>>>server socket listen failed<<<<");
        return;
    }
    LOGD(">>>server socket listen ok<<<<<");
    int maxfd;
    fd_set fdsr;
    int count = 0;
    while(1) {
        FD_ZERO(&fdsr);
        //add standard input
//        FD_SET(0, &fdsr);
        FD_SET(server_sock_fd, &fdsr);
        maxfd = server_sock_fd;

        struct timeval timeout = {3, 0};
        int ret = select(maxfd + 1, &fdsr, NULL, NULL, &timeout);
        if (ret < 0) {
            if (errno == EINTR) {
                LOGD(">>>>server timeout EINTR %d<<<<<", ++count);
                continue;
            }
            LOGE(">>>>server select error<<<<");
            return;
        } else if (ret == 0) {
            LOGD(">>>>server timeout %d<<<<<", ++count);
            continue;
        }

        if(FD_ISSET(server_sock_fd, &fdsr))
        {
            LOGD(">>>>begin accept client<<<<<");
            struct sockaddr_un client_address;
            socklen_t address_len = sizeof(client_address);
            int client_sock_fd = accept(server_sock_fd,(struct sockaddr *)&client_address, &address_len);
            LOGD(">>>>one client accept<<<<");

            if(client_sock_fd > 0)
            {
                char resv_message[BUFF_SIZE];
                bzero(resv_message,BUFF_SIZE);
                LOGD(">>>>begin listen client status<<<<");
                read(client_sock_fd, resv_message, BUFF_SIZE);
                LOGD(">>>>client disconnect now<<<<");
                //......
                if(execlp("am", "am", "-n", "com.bfy.linuxsocketdaemon/com.bfy.linuxsocketdaemon.DaemonService") != -1) {
                    LOGD(">>>>>restart DaemonService<<<<<<");
                } else {
                    LOGE(">>>>>>execute am cmd error errno= %s<<<<<", errno);
                }
                close(server_sock_fd);
                return;
            }
        }

    }
}

char* jstringToChar(JNIEnv* env, jstring jstr) {
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char*) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

jstring charTojstring(JNIEnv* env, const char* str) {
    jclass strClass = env->FindClass("Ljava/lang/String;");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(strlen(str));
    env->SetByteArrayRegion(bytes, 0, strlen(str), (jbyte*)str);
    jstring encoding = env->NewStringUTF("utf-8");
    return (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
}