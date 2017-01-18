#pragma once
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <condition_variable>
#include <functional>
#include "third_party/chromium/base/basictypes.h"
#include "third_party/chromium/base/timer/timer.h"
#include "third_party/chromium/base/threading/thread.h"
#include "third_party/chromium/base/task_runner.h"
#include "Network/IpProxy.h"
//#include "Network/TcpClient.h"
//#include "Network/TcpProxyClient.h"
#include "Network/TcpDefines.h"
//#include "Network/SingleTcpClient.h"
#include "Network/BetData.h"

//class TcpClient;
class TcpClientController;

//typedef int SocketHandle;
// 201消息回来解析后出去的数据包
struct EnterRoomUserInfo 
{
    std::string nickname = "";
    uint32 richlevel = 0;
    uint32 roomid = 0;
    uint32 unixtime = 0;
    uint32 userid = 0;
    bool vip_v = false; // vipData中v字段中 1为普通vip, 2为白金vip，可以开启隐身功能的。
    bool visable = true; // 是否或见，默认是可见，如果开了隐身则不可见
    bool isAdmin = false;
    bool isUserGuard = false;
    bool isGoldFans = false;
};

struct RoomChatMessage
{
    uint32 roomid = 0;
    uint32 senderid = 0;
    uint32 richlevel = 0;
    std::string sendername = "";
    uint32 receiverid = 0;
    std::string receivername = "";
    bool issecrect = false;
    std::string chatmessage = "";
};



struct RoomGiftInfo601
{
    uint32 time;
    uint32 roomid;
    uint32 senderid;
    std::string sendername;
    uint32 receiverid;
    std::string receivername;
    uint32 giftid;
    std::string giftname;
    uint32 gitfnumber;
    std::string tips;
    uint32 happyobj;
    uint32 happytype;
    std::string token;
};

typedef std::function<void(const RoomGiftInfo601& roomgiftinfo)> Notify601;
typedef std::function<void(const BetResult& bet_result)> Notify620;
typedef std::function<void(const EnterRoomUserInfo& enterRoomUserInfo)> Notify201;
typedef std::function<void(const EnterRoomUserInfo& enterRoomUserInfo,
    const RoomChatMessage& roomChatMessage)> Notify501;

typedef std::function<void(const std::wstring& data)> NormalNotify;
class MessageNotifyManager 
    : public std::enable_shared_from_this <MessageNotifyManager>
{
public:
    MessageNotifyManager();
    ~MessageNotifyManager();

    static void AddRef() {}
    static void Release() {}

    bool Initialize(const scoped_refptr<base::TaskRunner>& runner);
    void Finalize();
    void SetTcpManager(TcpClientController* tcpManager);
    void SetServerIp(const std::string& serverip);
    void SetIpProxy(const IpProxy& ipproxy);

    // 可动态改变的功能，都要转到工作线程去完成
    void SetNotify201(Notify201 notify201);
    void SetNotify501(Notify501 notify201);
    void SetNotify601(Notify601 notify601);
    void SetNotify620(Notify620 notify_620);
    void SetNormalNotify(NormalNotify normalNotify);

    // 使用新的高效的多路tcp请求分发模式
    bool NewConnect843(uint32 roomid, uint32 userid,
        const std::string& usertoken, 
        const base::Callback<void()>& conn_break_callback);
    bool NewConnect8080(uint32 roomid, uint32 userid, const std::string& usertoken);
    bool NewSendChatMessage(const std::string& nickname, uint32 richlevel,
                         const std::string& message);

    bool NewSendChatMessageRobot(const RoomChatMessage& roomChatMessage);

private:

    void DoSetNotify201(Notify201 notify201);
    void DoSetNotify501(Notify501 notify201);
    void DoSetNotify601(Notify601 notify601);
    void DoSetNotify620(Notify620 notify_620);
    void DoSetNormalNotify(NormalNotify normalNotify);

    void Notify(const std::vector<char>& data);
    std::vector<std::string> HandleMixPackage(const std::string& package);

    // 使用新的高效的多路tcp请求分发模式
    static void NewConnect843Callback(std::weak_ptr<MessageNotifyManager> weakptr,
        bool result, SocketHandle handle);
    static void NewConnect8080Callback(std::weak_ptr<MessageNotifyManager> weakptr, 
        uint32 roomid, uint32 userid,
        const std::string& usertoken,
        bool result, SocketHandle handle);
    static void NewData843Callback(std::weak_ptr<MessageNotifyManager> weakptr, 
        uint32 roomid, uint32 userid, const std::string& usertoken, bool result, 
        const std::vector<uint8>& data);
    static void NewData8080Callback(std::weak_ptr<MessageNotifyManager> weakptr, 
        bool result, const std::vector<uint8>& data);
    static void NewSendDataCallback(std::weak_ptr<MessageNotifyManager> weakptr, 
        SocketHandle handle, bool result);

    void DoNewConnect843Callback(bool result, SocketHandle handle);
    void DoNewConnect8080Callback(uint32 roomid, uint32 userid,
                             const std::string& usertoken,
                             bool result, SocketHandle handle);
    void DoNewData843Callback(uint32 roomid, uint32 userid,
        const std::string& usertoken, bool result, const std::vector<uint8>& data);
    void DoNewData8080Callback(bool result, const std::vector<uint8>& data);

    void DoNewSendHeartBeat(SocketHandle handle);
    void DoNewSendChatMessage(const std::vector<char>& msg); // 发言需要符合间隔时间
    void DoNewSendDataCallback(SocketHandle handle, bool result);

    //base::Thread baseThread_;
    scoped_refptr<base::TaskRunner> runner_;
    base::RepeatingTimer<MessageNotifyManager> repeatingTimer_;
    std::string serverip_ = "192.168.0.1";
    IpProxy ipProxy_;
    std::string Packet_ = "";
    int position_ = 0;

    //std::unique_ptr<TcpClient> tcpClient_8080_;
    //std::unique_ptr<TcpClient> tcpClient_843_;

    SocketHandle SocketHandle_8080_ = -1;
    SocketHandle SocketHandle_843_ = -1;

    Notify201 notify201_;
    Notify501 notify501_;
    Notify601 notify601_;
    Notify620 notify_620_;

    NormalNotify normalNotify_;

    TcpClientController* tcp_client_controller_;
    base::Callback<void()> conn_break_callback_; // 处理掉线问题
    bool connected_;// 未连接时和连接失败后，都不应该处理回调和线程任务

    base::RepeatingTimer<MessageNotifyManager> newRepeatingTimer_;
    base::TimeDelta chat_message_space_;
    base::Time last_chat_time_;

    // 屠龙相关数据
    std::vector<BetShowData> bet_show_datas_;
};

