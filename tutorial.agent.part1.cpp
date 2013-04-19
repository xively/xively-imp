const FEED_ID = "FEED_ID_HERE";
const API_KEY = "API_KEY_HERE";
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
    //if data is unformatted this may help you format multiple
	//csv transmits
    //local cdex = feedCSV.indexof(",");
    //local ds = feedCSV.substring(0,cdex);
    //local ndex = feedCSV.indexof("\n");
    //local val = feedCSV.substring(cdex,ndex);
    
    //send preformatted multi-ds csv
    send_cosm(feedCSV);
});