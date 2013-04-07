//basi sensor testing
hardware.pin1.configure(ANALOG_IN); 
local voltage = hardware.voltage();
server.log("Hardware Configured");
server.log(format("Running at %.2f V", voltage));

//////////setup vars///////////////////////////////////////
local readTime = 100;   //time in ms
local arraySize = 100; //readings per sample
local lowThresh = 8;    //default is 5
local vibratioThresh = 70;   //check vibration minimum
local wakeVibratio = 50;    //wake from sleep vibration
//sleep
local delayMS = 10000;  //delay time in MS

//cosm stuff
const SECRET = "ee2ab38f9b85b6b2c1b150b1d416b3fa2683dc72";      //product secret
const SERIAL = "aaa001";      //device serial number
///////////////////////////////////////////////////////////

 //vars//////////////
 local z0 = 0;  //sensor reading var
 local zLast = 0;   //last reading
 local zDelta = 0;  //calced
 
 local zValues = [arraySize];
 local zVibs = [10];
 local runKeeper = [10];
 local index = 0;   //handy
 
 local newArray = false;
 local newData = 0;
 local firstcheck = false;
 
 ///////////////////////
 
function checkVibs() {
    imp.wakeup(30.0, checkVibs);
    
    local checkedVib = takeVibratio();
    server.log("checkedVib="+checkedVib);
    
    if(checkedVib > wakeVibratio){
        server.log("woken");
        if(runDetect()){
            agent.send("run",hardware.millis());
            server.log("going to sleep");
            server.log("sleep currently disabled");
            //imp.onidle(function() { server.sleepfor(30.0); } );
            //server.sleepfor(30.0);
        }
    }
 
    
}

function takeVibratio(){
    server.log("take vib->" + hardware.millis())
    local xVibratio = 0.0;
    
    while(!newData){
        if(z0 != (hardware.pin1.read()/64)){
            z0 = (hardware.pin1.read()/64);
            zDelta = math.abs(zLast - z0);
            zValues.insert(index,zDelta);
            index++;
            //server.log(zDelta);
            zLast = z0;
            
            //sleep
            imp.sleep(0.02);
        }
        
        if(index>100){
            newData = true;
        }
    }
    
    if(newData){
        server.log("newData ->" + hardware.millis());
        
        local lowDelta = 0;
        
        foreach(val in zValues){
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
        zValues.clear();
        index = 0;
    }
    
    return xVibratio;
}

function runDetect(){
    local boolRunning = 0;
    
    for(local i = 0;i < 10; i++){
        zVibs.insert(i,takeVibratio());
        if(zVibs[i] >= vibratioThresh){
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