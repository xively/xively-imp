local feedID;
local apiKey;
local actTable;

function activateDevice(act){
    server.log("starting activate");
    server.log(act[0]);
    server.log(act[1]);
        local hash_url = "https://stringycode.com/dev/arduino-cosm/activationhash.php?secret=" + act[0] + "&serial=" + act[1];
        local hash_req = http.get(hash_url,{});
        local hash_res = hash_req.sendsync();
        server.log(hash_res.body);
        local act_url = "https://api.cosm.com/v2/devices/" + hash_res.body + "/activate";
        server.log(act_url);
        imp.sleep(1.0);
        local act_req = http.get(act_url,{});
        local act_res = act_req.sendsync();
        server.log(act_res.body);
        local actData = http.jsondecode(act_res.body);
        device.send("actSuccess", actData);
        
        //init datastreams
        server.log("initing datastreams");
        local cosm_url = "https://api.cosm.com/v2/feeds/" + actData.feed_id + ".csv";
        server.log(cosm_url);
        local body = "" + actData.datastreams[0] + ",0\n" + actData.datastreams[1] + ",0\n" + actData.datastreams[2] + ",0";
        local req = http.put(cosm_url, {"X-ApiKey":actData.apikey, "Content-Type":"text/plain", "User-Agent":"Cosm-Imp-Lib/1.0"}, body);
        local res = req.sendsync();
        if(res.statuscode != 200) {
            server.log("error sending message: "+res.body);
        }else server.log("init? " + res.body);
}

function get_cosm(millis) {
    local cosm_url = "https://api.cosm.com/v2/feeds/" + feedID + ".json";
    server.log(cosm_url);
    local getreq = http.get(cosm_url, {"X-ApiKey":apiKey, "User-Agent":"Cosm-Imp-Lib/1.0"});
    local res = getreq.sendsync();
    if(res.statuscode != 200) {
        server.log("error sending message: " + res.body);
    }
     
    server.log("#####RESPONSE#####");
    //server.log(res.body);
    local resTable = http.jsondecode(res.body);
    
    local values = [3];
    //update cycles
    values.insert(0,(resTable.datastreams[0].current_value.tointeger() + 1));
    //update lastrun
    values.insert(1, millis);
    //update onHand
    values.insert(2,(resTable.datastreams[2].current_value.tointeger() - 1));
    
    send_cosm(values);
}

function send_cosm(values) {
    local cosm_url = "https://api.cosm.com/v2/feeds/" + feedID + ".csv";
    server.log(cosm_url);
    local body = "";//+ datastream + "," + value;
    for(local i=0;i<3;i++){
        body = body + actTable.datastreams[i] + "," + values[i] + "\n";
    }
    local req = http.put(cosm_url, {"X-ApiKey":apiKey, "Content-Type":"text/csv", "User-Agent":"Cosm-Imp-Lib/1.0"}, body);
    local res = req.sendsync();
    if(res.statuscode != 200) {
        server.log("error sending message: "+res.body);
    }else server.log("cosm put - success")
}


////////////actual runtime//////////

device.on("run", function(millis) {
    server.log("device on");
    server.log(millis);
    get_cosm(millis);
});

device.on("activate", activateDevice);

device.on("setup", function(act) {
    server.log("setup on");
    //server.log(act.apikey);
    //server.log(act.feed_id);
    feedID = act.feed_id;
    apiKey = act.apikey;
    actTable = act;
});

device.on("fourier", function(raw) {
    server.log("doing fourier");
   local rawjson = http.jsonencode(raw);
   //server.log(rawjson);
    local fourier_url = "https://stringycode.com/dev/arduino-cosm/fourier.php";//?input=" + rawjson;
    //server.log(fourier_url);
    local getreq = http.post(fourier_url, {}, rawjson);
    local res = getreq.sendsync();
    server.log(res.body);
   
});