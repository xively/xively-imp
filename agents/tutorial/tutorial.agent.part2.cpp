const FEED_ID = "FEED_ID_HERE";
const API_KEY = "API_KEY_HERE";

function get_cosm() {
    //wakeup
    imp.wakeup(30.0, get_cosm);     //schedule when to wakeup again
     
    local cosm_url = "https://api.cosm.com/v2/feeds/" + FEED_ID + ".json";      //format cosm url
    server.log(cosm_url);
     
    local getreq = http.get(cosm_url, {"X-ApiKey":API_KEY, "User-Agent":"Cosm-Imp-Lib/1.0"});       //add headers
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

get_cosm();     //initialize first get request. will continue to call itself.

function send_cosm(body) {         //take in csv value
    local cosm_url = "https://api.cosm.com/v2/feeds/" + FEED_ID + ".csv";       //setup url for csv
    server.log(cosm_url);
    server.log(body);       //pring body for testing
    local req = http.put(cosm_url, {"X-ApiKey":API_KEY, "Content-Type":"text/csv", "User-Agent":"Cosm-Imp-Lib/1.0"}, body);     //add headers
    local res = req.sendsync();         //send request
    if(res.statuscode != 200) {
        server.log("error sending message: "+res.body);
    }else device.send("status", (res.statuscode + " OK"));      //sends status to uart. this can be removed if not desired

}

 
device.on("data", function(feedCSV) {       //take csv body in from device
    server.log("device on");
    
    //send preformatted multi-ds csv
    send_cosm(feedCSV);         //send to function to call cosm
});