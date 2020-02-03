--TEST--
SOAP Interop Round2 groupB 005 (soap/direct): echoNestedArray
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
--FILE--
<?php
$param = new SoapParam(new SoapVar(array(
    new SoapVar("arg", XSD_STRING, null, null, "varString"),
    new SoapVar(34, XSD_INT, null, null, "varInt"),
    new SoapVar(325.325, XSD_FLOAT, null, null, "varFloat"),
    new SoapVar(array(
        new SoapVar("red", XSD_STRING),
        new SoapVar("blue", XSD_STRING),
        new SoapVar("green", XSD_STRING),
    ), SOAP_ENC_ARRAY, "ArrayOfString", "http://soapinterop.org/xsd", 'varArray')
  ), SOAP_ENC_OBJECT, "SOAPArrayStruct", "http://soapinterop.org/xsd"), "inputStruct");
$client = new SoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));
$client->__soapCall("echoNestedArray", array($param), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_groupB.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoNestedArray><inputStruct xsi:type="ns2:SOAPArrayStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">325.325</varFloat><varArray SOAP-ENC:arrayType="xsd:string[3]" xsi:type="ns2:ArrayOfString"><item xsi:type="xsd:string">red</item><item xsi:type="xsd:string">blue</item><item xsi:type="xsd:string">green</item></varArray></inputStruct></ns1:echoNestedArray></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoNestedArrayResponse><return xsi:type="ns2:SOAPArrayStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">325.325</varFloat><varArray SOAP-ENC:arrayType="xsd:string[3]" xsi:type="ns2:ArrayOfstring"><item xsi:type="xsd:string">red</item><item xsi:type="xsd:string">blue</item><item xsi:type="xsd:string">green</item></varArray></return></ns1:echoNestedArrayResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
