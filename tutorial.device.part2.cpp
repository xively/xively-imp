local index = 0;
local masterUART = array(100);

//these should be changed to non character values
local startbit = 124;   //which is |
local endbit = 126;     //which is ~

//uart initialization
function startUART()
{
    hardware.configure(UART_57);
    hardware.uart57.configure(19200, 8, PARITY_NONE, 1, NO_CTSRTS);     //baud:19200, dataBits:8, parity, stopbit
}

function checkUART()
{
    imp.wakeup(0.001, checkUART.bindenv(this));
    
    local byte = hardware.uart57.read();    //read uart 
  
    while (byte != -1)  
    {
        //server.log(byte);         
        if(byte == endbit){     //check if endbit
            local startint = masterUART.find(startbit);     //find startbit
            if(startint != null){   
                local fullTrans = masterUART.slice((startint + 1),index);   //slice transmission from checkbits
                local transString = "";
                for(local a=0;a<fullTrans.len();a++){   //convert char array to string
                    //server.log(fullTrans[a]);
                    transString = transString + format("%c",fullTrans[a]); 
                }
                agent.send("data", transString);    //send string to agent
                fullTrans.clear();
                masterUART.clear();
                index = 0;
            }else server.log("UART ERROR");
        }else{  //if not endbit, add byte to array
            masterUART.insert(index,byte);
            index ++;
        }
        byte = hardware.uart57.read();         
    }
}

agent.on("json", function(value) {      //receive data from agent
    server.log("agent is on");
    //server.log(value);                //uncomment to print the data first
    
    hardware.uart57.write("\n" + value);        //write data to UART
});

agent.on("status", function(code) {     //this functions send the cosm put status code to serial port.
    server.log("agent is on");
    hardware.uart57.write("\n" + code);
});

imp.configure("Cosm UART", [], []);     //standard imp configure statement
startUART();    //setup uart
checkUART();    //begin uart polling