--TEST--
Fix #66049 Typemap can break parsing in parse_packet_soap leading to a segfault
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
function soap_string_from_xml($str)
    { 
    echo "soap_string_from_xml\n";

	// Should return an string
    return 2.3; 
    }

class TestSoapClient extends SoapClient {
    function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
        $res='<?xml version="1.0" encoding="UTF-8"?>
            <SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">
            <SOAP-ENV:Body>
            <SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>not present</faultstring>
            </SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>';
        return $res;
    }
}

try {
    $client=new TestSoapClient(null, [
        'uri' => 'test://',
        'location' => 'test://',
        'typemap' => [[
            "type_ns" => "http://www.w3.org/2001/XMLSchema",
            "type_name" => "string",
            "from_xml" => "soap_string_from_xml"
         ]]]);
    $client->Mist("");
} catch (SoapFault $e) {
    var_dump($e->faultstring);
    var_dump($e->faultcode);
}
?>
Done
--EXPECT--
soap_string_from_xml
string(3) "2.3"
string(15) "SOAP-ENV:Server"
Done
