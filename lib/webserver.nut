function webserver(request,res){
    local responseTable = http.urldecode(request.body);
    local parsedTable = http.jsondecode(responseTable.body);
    server.log(parsedTable.id);
    
    //PRINT TABLE VALUES
//    foreach(key, value in responseTable) {  
//        server.log("dataTable["+key+"] = "+value);
//    }
    //RETURN TRIGGER DS AND VALUE
    local trig = parsedTable.triggering_datastream.id + "," + parsedTable.triggering_datastream.value.value;
    //return to cosm
    res.send(200, "okay");
    //print to log - could also send to device
    server.log(trig);
}
//setup pieces - not part of function
server.log("access me via "+http.agenturl());
http.onrequest(webserver);