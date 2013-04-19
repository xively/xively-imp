local index = 0;
local masterUART = array(100);

//these should be changed to non ascii values
local startbit = 88;//which is X
local endbit = 81;//which is Q
//methods
function initUart()
{
    hardware.configure(UART_57);
    hardware.uart57.configure(19200, 8, PARITY_NONE, 1, NO_CTSRTS); // 19200 baud
}

function pollUart()
{
    imp.wakeup(0.001, pollUart.bindenv(this));
    
    local byte = hardware.uart57.read();    // read the UART buffer
  
    while (byte != -1)  
    {
              
        if(byte == endbit){     //byte 81 is character Q
            //server.log("master:\n");
            //server.log(masterUART);
            local startint = masterUART.find(startbit);
            if(startint != null){
                local fullTrans = masterUART.slice((startint + 1),index);
                local transString = "";
                for(local a=0;a<fullTrans.len();a++){
                    //server.log(fullTrans[a]);
                    //server.log(format("%c",fullTrans[a]));
                    transString = transString + format("%c",fullTrans[a]);
                }
                agent.send("data", transString);
                fullTrans.clear();
                masterUART.clear();
                index = 0;
            }else server.log("UART ERROR");
        }else{
            masterUART.insert(index,byte);
            index ++;
        }
        byte = hardware.uart57.read();         
    }
}


agent.on("json", function(value) {
    server.log("agent is on");
    server.log(value);
    
    hardware.uart57.write("\n" + value);
});

imp.configure("Cosm UART", [], []);
initUart(); 
pollUart();