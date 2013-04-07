local feedID;
local apiKey;

function activateDevice(act){
    server.log("starting activate");
    server.log(act[0]);
    server.log(act[1]);
        local hash_url = "http://stringycode.com/dev/arduino-cosm/activationhash.php?secret=" + act[0] + "&serial=" + act[1];
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
}




////////////actual runtime//////////

device.on("run", function(millis) {
    server.log("device on");
    server.log(millis);
});

device.on("activate", activateDevice);

device.on("setup", function(act) {
    server.log("setup on");
    server.log(act.apikey);
    server.log(act.feed_id);
    server.log(act.datastreams[0]);
    feedID = act.feed_id;
    apiKey = act.apikey;
});