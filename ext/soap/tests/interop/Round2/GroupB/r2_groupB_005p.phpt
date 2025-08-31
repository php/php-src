--TEST--
SOAP Interop Round2 groupB 005 (php/direct): echoNestedArray
--EXTENSIONS--
soap
--INI--
precision=14
--FILE--
<?php
$param = (object)array(
  'varString'=>'arg',
  'varInt'=>34,
  'varFloat'=>325.325,
  'varArray' => array('red','blue','green'));
$client = new SoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));
$client->__soapCall("echoNestedArray", array($param), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_groupB.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoNestedArray><param0 xsi:type="SOAP-ENC:Struct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">325.325</varFloat><varArray SOAP-ENC:arrayType="xsd:string[3]" xsi:type="SOAP-ENC:Array"><item xsi:type="xsd:string">red</item><item xsi:type="xsd:string">blue</item><item xsi:type="xsd:string">green</item></varArray></param0></ns1:echoNestedArray></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoNestedArrayResponse><return xsi:type="ns2:SOAPArrayStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">325.325</varFloat><varArray SOAP-ENC:arrayType="xsd:string[3]" xsi:type="ns2:ArrayOfstring"><item xsi:type="xsd:string">red</item><item xsi:type="xsd:string">blue</item><item xsi:type="xsd:string">green</item></varArray></return></ns1:echoNestedArrayResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
