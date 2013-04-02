//basi sensor testing
hardware.pin1.configure(ANALOG_IN); 
local voltage = hardware.voltage();
server.log("Hardware Configured");
server.log(format("Running at %.2f V", voltage));

//////////setup vars///////////////////////////////////////
local readTime = 100;   //time in ms
local arraySize = 100; //readings per sample
local lowThresh = 3;
local vibratioThreshold = 50;   //check vibration minimum
local wakeVibratio = 50;    //wake from sleep vibration
//sleep
local delayMS = 10000;  //delay time in MS

///////////////////////////////////////////////////////////

 //vars//////////////
 local z0 = 0;  //sensor reading var
 local zLast = 0;   //last reading
 local zDelta = 0;  //calced
 
 local zValues = [arraySize];
 local index = 0;   //handy
 
 local newArray = false;
 local newData = 0;
 local firstcheck = false;
 
 ///////////////////////
 
function checkVibs() {
    imp.wakeup(5.0, checkVibs);
    
    server.log(takeVibratio());
   
 
    
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
        xVibratio = hardware.pin1.read();
        local lowDelta = 0;
        
        foreach(val in zValues){
            if(val <= lowThresh){
                lowDelta++;
            }
        }
        lowDelta--;
        

        server.log(lowDelta);
        server.log(arraySize);
        local sizef = arraySize.tofloat();
        xVibratio = ((arraySize - lowDelta) / sizef) * 100;
        server.log(xVibratio);
        
        newData = 0;
        zValues.clear();
        index = 0;
    }
    return xVibratio;
}



//starters 
server.log("Accelerometer Started");
imp.configure("AppMon", [], []);
checkVibs();