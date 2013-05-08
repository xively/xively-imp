function get_raw(feed_id, api_key, format) {
    local ret = "";
    local xively_url = "https://api.cosm.com/v2/feeds/" + feed_id + "." + format;
    local getreq = http.get(xively_url, {"X-ApiKey":api_key, "User-Agent":"Cosm-Imp-Lib/1.0"});
    local res = getreq.sendsync();
    if(res.statuscode != 200) {
        ret = "error sending message: " + res.body;
    }else ret = res.body;
    
    return ret;
}