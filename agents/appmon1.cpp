//basi sensor testing
hardware.pin1.configure(ANALOG_IN); 
hardware.pin8.configure(ANALOG_IN); 
hardware.pin9.configure(ANALOG_IN); 
local voltage = hardware.voltage();
server.log("Hardware Configured");
server.log(format("Running at %.2f V", voltage));

//////////setup vars///////////////////////////////////////
local readTime = 100;   //time in ms
local arraySize = 100; //readings per sample
local lowThresh = 5;    //default is 5
local vibratioThresh = 40;   //check vibration minimum
local wakeVibratio = 30;    //wake from sleep vibration
//sleep
local delayMS = 10000;  //delay time in MS

//xively stuff
const SECRET = "ee2ab38f9b85b6b2c1b150b1d416b3fa2683dc72";      //product secret
const SERIAL = "aaa001";      //device serial number
///////////////////////////////////////////////////////////

 //vars//////////////
 local z0 = 0;  //sensor reading var
 local x0 = 0;
 local y0 = 0;
 local ab0 = 0;
 
 local abLast = 0;   //last reading
 local abDelta = 0;  //calced
 
 local abValues = [arraySize];
 local abRaw = [arraySize];
 local abVibs = [10];
 local runKeeper = [10];
 local index = 0;   //handy
 
 local newArray = false;
 local newData = 0;
 local firstcheck = false;
 
 ///////////////////////
 
function checkVibs() {
    imp.wakeup(30.0, checkVibs);
    
    local checkedVib = takeVibratio();
    //server.log("checkedVib="+checkedVib);
    
    if(checkedVib > wakeVibratio){
        server.log("woken");
        if(runDetect()){
            agent.send("run",hardware.millis());
            server.log("going to sleep");
            server.log("sleep currently disabled");
            imp.onidle(function(){ 
                server.sleepfor(30.0);
            });
            //server.sleepfor(30.0);
        }
    }
 
    
}

function takeVibratio(){
    server.log("take vib->" + hardware.millis())
    local xVibratio = 0.0;
    
    while(!newData){
        if(1 == 1){//z0 != (hardware.pin1.read()/64)){
            z0 = (hardware.pin1.read()/64);
            x0 = (hardware.pin9.read()/64);
            y0 = (hardware.pin8.read()/64);
            ab0 = math.sqrt((z0*z0) + (x0*x0) + (y0*y0));
            //server.log(ab0);
            abRaw.insert(index,ab0);
            abDelta = math.abs(abLast - ab0);
            abValues.insert(index,abDelta);
            index++;
            //server.log(abDelta);
            abLast = ab0;
            
            //sleep
            imp.sleep(0.01);
        }
        
        if(index>100){
            newData = true;
        }
    }
    
    if(newData){
        server.log("newData ->" + hardware.millis());
        agent.send("fourier", abRaw);
        local lowDelta = 0;
        
        foreach(val in abValues){
            if(val <= lowThresh){
                lowDelta++;
            }
        }
        lowDelta--;
        

        // server.log(lowDelta);
        //server.log(arraySize);
        local sizef = arraySize.tofloat();
        xVibratio = ((arraySize - lowDelta) / sizef) * 100;
        server.log("vibratio:" + xVibratio);
        
        newData = 0;
        abValues.clear();
        index = 0;
    }
    
    return xVibratio;
}

function runDetect(){
    server.log("detecting run");
    local boolRunning = 0;
    
    for(local i = 0;i < 10; i++){
        abVibs.insert(i,takeVibratio());
        if(abVibs[i] >= vibratioThresh){
            runKeeper.insert(i,1);
        }else runKeeper.insert(i,0);
    }
    
    local vib1count = 0;
    local vib0count = 0;
    foreach(val in runKeeper){
        if(val == 1){
            vib1count++;
        }else vib0count++;
    }
    if((vib1count - vib0count) >= 2){
        server.log("run has been detected");
        boolRunning = true;
    }else server.log("no run detected in runDetect()")
    
    return boolRunning;
}

function setupDevice(){
    server.log("starting setup");
    if("actData" in server.permanent){//"feedID2" in server.permanent && "apiKey2" in server.permanent){
        server.log("feed and key found");
        //local feedArray = [server.permanent.feedID, server.permanent.apiKey];
        agent.send("setup", server.permanent.actData);
    }else{
        server.log("feed not found. activating...");
        local actArray = [SECRET,SERIAL];
        agent.send("activate", actArray);
    } 
}

//starters
agent.on("actSuccess", function(actData) {
    server.log("act is success");
    server.log(actData.apikey);
    server.log(actData.feed_id);
    server.setpermanentvalues({actData = actData});
});

//server.log("Accelerometer Started");
imp.configure("AppMon", [], []);
setupDevice();
checkVibs();
server.log("id: " + hardware.getimpeeid());
//agent.send("run",hardware.millis());