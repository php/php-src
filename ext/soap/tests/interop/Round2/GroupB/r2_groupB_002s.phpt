--TEST--
SOAP Interop Round2 groupB 002 (soap/direct): echoSimpleTypesAsStruct
--EXTENSIONS--
soap
--INI--
precision=14
--FILE--
<?php
$client = new SoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));
$client->__soapCall("echoSimpleTypesAsStruct", array(
  new SoapParam(new SoapVar("arg",XSD_STRING), "inputString"),
  new SoapParam(new SoapVar(34,XSD_INT), "inputInteger"),
  new SoapParam(new SoapVar(34.345,XSD_FLOAT), "inputFloat")), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_groupB.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoSimpleTypesAsStruct><inputString xsi:type="xsd:string">arg</inputString><inputInteger xsi:type="xsd:int">34</inputInteger><inputFloat xsi:type="xsd:float">34.345</inputFloat></ns1:echoSimpleTypesAsStruct></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoSimpleTypesAsStructResponse><return xsi:type="ns2:SOAPStruct"><varString xsi:type="xsd:string">arg</varString><varInt xsi:type="xsd:int">34</varInt><varFloat xsi:type="xsd:float">34.345</varFloat></return></ns1:echoSimpleTypesAsStructResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
