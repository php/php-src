--TEST--
SOAP Interop Round2 groupB 001 (php/direct): echoStructAsSimpleTypes
--EXTENSIONS--
soap
--INI--
precision=14
--FILE--
<?php
$client = new SoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));
$client->__soapCall("echoStructAsSimpleTypes", array((object)array('varString'=>"arg",'varInt'=>34,'varFloat'=>34.345)), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_groupB.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStructAsSimpleTypes><param0 xsi:type="SOAP-ENC:Struct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">34.345</varFloat></param0></ns1:echoStructAsSimpleTypes></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStructAsSimpleTypesResponse><outputString xsi:type="xsd:string">arg</outputString><outputInteger xsi:type="xsd:int">34</outputInteger><outputFloat xsi:type="xsd:float">34.345</outputFloat></ns1:echoStructAsSimpleTypesResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
