--TEST--
SOAP Interop Round4 GroupH Simple RPC Enc 010 (php/wsdl): echoMultipleFaults2(3)
--EXTENSIONS--
soap
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/round4_groupH_simple_rpcenc.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoMultipleFaults2(3, "Hello World", 12.345, array("one","two","three"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupH_simple_rpcenc.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/wsdl" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:ns2="http://soapinterop.org/types" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoMultipleFaults2><whichFault xsi:type="xsd:int">3</whichFault><param1 xsi:type="xsd:string">Hello World</param1><param2 xsi:type="xsd:float">12.345</param2><param3 SOAP-ENC:arrayType="xsd:string[3]" xsi:type="ns2:ArrayOfString"><item xsi:type="xsd:string">one</item><item xsi:type="xsd:string">two</item><item xsi:type="xsd:string">three</item></param3></ns1:echoMultipleFaults2></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns1="http://soapinterop.org/types" xmlns:ns2="http://soapinterop.org/wsdl" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Fault in response to 'echoMultipleFaults2'.</faultstring><detail><ns2:part6 SOAP-ENC:arrayType="xsd:string[3]" xsi:type="ns1:ArrayOfString"><item xsi:type="xsd:string">one</item><item xsi:type="xsd:string">two</item><item xsi:type="xsd:string">three</item></ns2:part6></detail></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
