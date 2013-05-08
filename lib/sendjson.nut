function send_xively(feed_id, api_key, body) {         //take in csv value
    local ret = "";
    local cosm_url = "https://api.cosm.com/v2/feeds/" + feed_id + ".json";       //setup url for csv
    local request = http.put(cosm_url, {"X-ApiKey":api_key, "Content-Type":"text/csv", "User-Agent":"Cosm-Imp-Lib/1.0"}, body);     //add headers
    local resp = request.sendsync();         //send request
    if(resp.statuscode != 200) {
        ret = "error sending message: "+resp.body;
    }else ret = "200 OK";
    
    return ret;
}