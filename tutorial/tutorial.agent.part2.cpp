const FEED_ID = "FEED_ID_HERE";
const API_KEY = "API_KEY_HERE";

function get_xively() {
    //wakeup
    imp.wakeup(30.0, get_xively);     //schedule when to wakeup again
     
    local xively_url = "https://api.xively.com/v2/feeds/" + FEED_ID + ".json";      //format xively url
    server.log(xively_url);
     
    local getreq = http.get(xively_url, {"X-ApiKey":API_KEY, "User-Agent":"Xively-Imp-Lib/1.0"});       //add headers
    local res = getreq.sendsync();      //actuallty send request
    if(res.statuscode != 200) {
        server.log("error sending message: " + res.body);
    }
    server.log("#####RESPONSE#####");
    server.log(res.statuscode + " OK");     //status
    server.log(res.body);                   //body
    //format data into table - uncomment to use
//    local resTable = http.jsondecode(res.body);
//    server.log("table:");
//    server.log(resTable.title);
//    server.log(resTable.id);
//    //print ds0
//    server.log(resTable.datastreams[0].id);
//    server.log(resTable.datastreams[0].current_value + " at " + resTable.datastreams[0].at);
    
    
    //send to device
    //device.send("value", resTable.datastreams[0].current_value);      //send custom data from table
    
    device.send("json", res.body);     //send json
}

get_xively();     //initialize first get request. will continue to call itself.

function send_xively(body) {         //take in csv value
    local xively_url = "https://api.xively.com/v2/feeds/" + FEED_ID + ".csv";       //setup url for csv
    server.log(xively_url);
    server.log(body);       //pring body for testing
    local req = http.put(xively_url, {"X-ApiKey":API_KEY, "Content-Type":"text/csv", "User-Agent":"Xively-Imp-Lib/1.0"}, body);     //add headers
    local res = req.sendsync();         //send request
    if(res.statuscode != 200) {
        server.log("error sending message: "+res.body);
    }else device.send("status", (res.statuscode + " OK"));      //sends status to uart. this can be removed if not desired

}

 
device.on("data", function(feedCSV) {       //take csv body in from device
    server.log("device on");
    
    //send preformatted multi-ds csv
    send_xively(feedCSV);         //send to function to call xively
});