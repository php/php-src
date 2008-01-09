--TEST--
SOAP Interop Round4 GroupH Simple RPC Enc 014 (php/wsdl): echoMultipleFaults3(3)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(dirname(__FILE__)."/round4_groupH_simple_rpcenc.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoMultipleFaults3(3,"arg1","arg2");
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupH_simple_rpcenc.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/wsdl" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoMultipleFaults3><whichFault xsi:type="xsd:int">3</whichFault><param1 xsi:type="xsd:string">arg1</param1><param2 xsi:type="xsd:string">arg2</param2></ns1:echoMultipleFaults3></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns1="http://soapinterop.org/wsdl/fault1" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Fault in response to 'echoMultipleFaults3'.</faultstring><detail><ns1:part2 xsi:type="xsd:string">arg1</ns1:part2></detail></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
