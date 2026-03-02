--TEST--
SOAP Interop Round3 GroupD RPC Encoded 002 (php/wsdl): echoStringArray
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/round3_groupD_rpcenc.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoStringArray(array("one","two","three"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round3_groupD_rpcenc.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/WSDLInteropTestRpcEnc" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStringArray><param0 SOAP-ENC:arrayType="xsd:string[3]" xsi:type="ns2:ArrayOfstring"><item xsi:type="xsd:string">one</item><item xsi:type="xsd:string">two</item><item xsi:type="xsd:string">three</item></param0></ns1:echoStringArray></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/WSDLInteropTestRpcEnc" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStringArrayResponse><return SOAP-ENC:arrayType="xsd:string[3]" xsi:type="ns2:ArrayOfstring"><item xsi:type="xsd:string">one</item><item xsi:type="xsd:string">two</item><item xsi:type="xsd:string">three</item></return></ns1:echoStringArrayResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
