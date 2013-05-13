function get_xively(feed_id, api_key) {
    local xively_url = "https://api.xively.com/v2/feeds/" + feed_id + ".json";
    local getreq = http.get(xively_url, {"X-ApiKey":api_key, "User-Agent":"Xively-Imp-Lib/1.0"});
    local res = getreq.sendsync();
    if(res.statuscode != 200) {
        server.log("error sending message: " + res.body);
    }else{
        local resTable = http.jsondecode(res.body);
        return resTable;
    }
}