const FEED_ID = "119865";
const API_KEY = "qwgFCfpFeiKDzZjFlPvLrclFTPKSAKwvOGdhYmZmWFB0ND0g";
 
function get_cosm() {
    //wakeup
    imp.wakeup(15.0, get_cosm);
     
    local cosm_url = "https://api.cosm.com/v2/feeds/" + FEED_ID + ".json";
    server.log(cosm_url);
     
    local getreq = http.get(cosm_url, {"X-ApiKey":API_KEY, "User-Agent":"Cosm-Imp-Lib/1.0"});
    local res = getreq.sendsync();
    if(res.statuscode != 200) {
        server.log("error sending message: " + res.body);
    }
    server.log("#####RESPONSE#####");
    server.log(res.body);
    local resTable = http.jsondecode(res.body);
    server.log("table:");
    server.log(resTable.title);
    server.log(resTable.id);
    


    //print ds0
    server.log(resTable.datastreams[0].id);
    server.log(resTable.datastreams[0].current_value + " at " + resTable.datastreams[0].at);
    //send to device
    //device.send("value", resTable.datastreams[0].current_value);
    device.send("json", http.jsonencode(resTable));     //send json
}

function send_cosm(value) {
    local cosm_url = "https://api.cosm.com/v2/feeds/" + FEED_ID + ".csv";
    server.log(cosm_url);
    server.log(value);
    local body = value;//"" + datastream + "," + value;
    local req = http.put(cosm_url, {"X-ApiKey":API_KEY, "Content-Type":"text/csv", "User-Agent":"Cosm-Imp-Lib/1.0"}, body);
    local res = req.sendsync();
    if(res.statuscode != 200) {
        server.log("error sending message: "+res.body);
    }
}
 
device.on("data", function(feedCSV) {
    server.log("device on");
    //send preformatted multi-ds csv
    send_cosm(feedCSV);
});

get_cosm();