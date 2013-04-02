//basi sensor testing
hardware.pin1.configure(ANALOG_IN);
 
// you can read the imp's input voltage at any time with:
local voltage = hardware.voltage();
server.log(format("Running at %.2f V", voltage));
 
// We use this variable to create a "dead band" around the current value on the potentiometer
// this can be used to decrease the rate at which we push data to planner by discarding values that
// are the same as the last one
local lastRawValue = 0;
 
server.log("Hardware Configured");
 local servarray = [100];
 local index = 0;
function checkPot() {
    imp.wakeup(0.1, checkPot);
    
    local rawValue = hardware.pin1.read();
    if (1){//math.abs(rawValue - lastRawValue) > 150) {
        local potValue = rawValue/64;// / 65535.0;
        lastRawValue = rawValue;
        // note that we divide by 65535.0 to get a value between 0.0 and 1.0
        //server.log(potValue);
        
        if(index > 10){
            local tot = 0;
            foreach(val in servarray){
                //server.log(val);
                tot = tot + val;
            }
            server.log(tot / servarray.len());
            servarray.clear();
            index= 0;            
        } else {
            servarray.insert(index,potValue);
            index = index + 1;
        }
       
    }
 
    
}
 
server.log("Potentiometer Started");
imp.configure("Potentiometer", [], []);
checkPot();