--TEST--
SOAP Interop Round2 base 011 (php/direct): echoIntegerArray
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$client = new SoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));
$client->__soapCall("echoIntegerArray", array(array(1,234324324,2)), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_base.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoIntegerArray><param0 SOAP-ENC:arrayType="xsd:int[3]" xsi:type="SOAP-ENC:Array"><item xsi:type="xsd:int">1</item><item xsi:type="xsd:int">234324324</item><item xsi:type="xsd:int">2</item></param0></ns1:echoIntegerArray></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoIntegerArrayResponse><outputIntegerArray SOAP-ENC:arrayType="xsd:int[3]" xsi:type="ns2:ArrayOfint"><item xsi:type="xsd:int">1</item><item xsi:type="xsd:int">234324324</item><item xsi:type="xsd:int">2</item></outputIntegerArray></ns1:echoIntegerArrayResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
