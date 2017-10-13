#include "SingerLocation.h"
#include "Network/easy_http_impl.h"
#include "Network/EncodeHelper.h"
#include "Network/CookiesHelper.h"
#include "third_party/chromium/base/callback.h"
#include "third_party/chromium/base/bind.h"
#include "third_party/chromium/base/strings/utf_string_conversions.h"

namespace{
bool MakePostdata(const std::map<std::string, std::string>& postmap,
    std::vector<uint8>* postdata)
{
    if (postmap.empty())
        return false;

    std::string temp;
    bool first = true;
    for (const auto& param : postmap)
    {
        if (first)
            first = false;
        else
            temp += "&";

        temp += param.first + "=" + UrlEncode(param.second);
    }
    postdata->assign(temp.begin(), temp.end());
    return true;
}
}
SingerLocation::SingerLocation()
{
}


SingerLocation::~SingerLocation()
{
}

void SingerLocation::Test()
{
    HttpRequest request1;
    request1.url = "https://jinshuju.net/f/orKrbn";

    //request.asyncHttpResponseCallback = base::Bind(&SingerLocation::HttpResponseCallback,
    //    base::Unretained(this));
    //easy_http_impl_->AsyncHttpRequest(request);

    request1.method = HttpRequest::HTTP_METHOD::HTTP_METHOD_GET;
    CurlWrapper curl_wrapper;
    HttpResponse response1;
    if (!curl_wrapper.Execute(request1, &response1))
        return;

    CookiesHelper cookies_helper;
    for (const auto& cookie : response1.cookies)
    {
        cookies_helper.SetCookies(cookie);
    }

    // 找csrf-token
    std::string content(response1.content.begin(),response1.content.end());
    auto token_pos = content.find("csrf-token");
    if (token_pos == std::string::npos)
    {
        return;
    }

    auto content_pos = content.find("content", token_pos);
    if (content_pos == std::string::npos)
    {
        return;
    }

    auto last_pos = content.find("/>", content_pos);

    std::string content_with_value = content.substr(content_pos, last_pos - content_pos);

    auto begin_pos = content_with_value.find("\"");
    auto end_pos = content_with_value.find("\"", begin_pos+1);

    std::string authenticity_token = content_with_value.substr(begin_pos + 1, end_pos - begin_pos);

    HttpRequest request2;
    request2.url = "https://jinshuju.net/f/orKrbn";
    request2.method = HttpRequest::HTTP_METHOD::HTTP_METHOD_POST;
    std::map<std::string, std::string> post_map;
    post_map["utf8"] = WideToUtf8(L"✓");
    post_map["authenticity_token"] = "authenticity_token";
    post_map["entry[field_4]"] = "6727";//公会id
    post_map["entry[field_17]"] = "1130681697";//主播id
    post_map["entry[field_32][province]"] = WideToUtf8(L"广东省");
    post_map["entry[field_32][city]"] = WideToUtf8(L"广州市");
    post_map["entry[field_32][district]"] = "";
    post_map["entry[field_32][street]"] = "";
    post_map["commit"] = WideToUtf8(L"提交");
    post_map["embedded"] = "";
    post_map["banner"] = "";
    post_map["header"] = "";
    post_map["background"] = "";
    MakePostdata(post_map, &request2.postdata);

    request2.cookies = cookies_helper.GetAllCookies();
    request2.referer = "https://jinshuju.net/f/orKrbn";

    HttpResponse response2;
    if (!curl_wrapper.Execute(request2, &response2))
        return;


    return;
}

void SingerLocation::HttpResponseCallback(const HttpResponse& response)
{
    if (response.statuscode != 200)
        return;
    
 
    return;
}
