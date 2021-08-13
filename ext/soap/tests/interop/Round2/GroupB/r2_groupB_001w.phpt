--TEST--
SOAP Interop Round2 groupB 001 (php/wsdl): echoStructAsSimpleTypes
--EXTENSIONS--
soap
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/round2_groupB.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoStructAsSimpleTypes((object)array('varString'=>"arg",'varInt'=>34,'varFloat'=>34.345));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_groupB.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStructAsSimpleTypes><inputStruct xsi:type="ns2:SOAPStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">34.345</varFloat></inputStruct></ns1:echoStructAsSimpleTypes></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStructAsSimpleTypesResponse><outputString xsi:type="xsd:string">arg</outputString><outputInteger xsi:type="xsd:int">34</outputInteger><outputFloat xsi:type="xsd:float">34.345</outputFloat></ns1:echoStructAsSimpleTypesResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
