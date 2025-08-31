--TEST--
SOAP Interop Round3 GroupD Import3 001 (php/wsdl): echoStruct
--EXTENSIONS--
soap
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
class SOAPStruct {
    function __construct(public $varString, public $varInt, public $varFloat) {}
}
$struct = new SOAPStruct('arg',34,325.325);
$client = new SoapClient(__DIR__."/round3_groupD_import3.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoStruct($struct);
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round3_groupD_import3.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStruct><inputStruct xsi:type="ns2:SOAPStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">325.325</varFloat></inputStruct></ns1:echoStruct></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStructResponse><Result xsi:type="ns2:SOAPStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">325.325</varFloat></Result></ns1:echoStructResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
