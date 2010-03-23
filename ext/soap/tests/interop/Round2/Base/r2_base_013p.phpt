--TEST--
SOAP Interop Round2 base 013 (php/direct): echoFloatArray
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
--FILE--
<?php
$client = new SoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));
$client->__soapCall("echoFloatArray", array(array(1.3223,34.2,325.325)), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_base.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoFloatArray><param0 SOAP-ENC:arrayType="xsd:float[3]" xsi:type="SOAP-ENC:Array"><item xsi:type="xsd:float">1.3223</item><item xsi:type="xsd:float">34.2</item><item xsi:type="xsd:float">325.325</item></param0></ns1:echoFloatArray></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoFloatArrayResponse><outputFloatArray SOAP-ENC:arrayType="xsd:float[3]" xsi:type="ns2:ArrayOffloat"><item xsi:type="xsd:float">1.3223</item><item xsi:type="xsd:float">34.2</item><item xsi:type="xsd:float">325.325</item></outputFloatArray></ns1:echoFloatArrayResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
