--TEST--
SOAP Interop Round2 base 006 (soap/direct): echoStringArray
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$param =  new SoapParam(new SoapVar(array(
    new SoapVar('good', XSD_STRING),
    new SoapVar('bad', XSD_STRING)
  ), SOAP_ENC_ARRAY, "ArrayOfstring","http://soapinterop.org/xsd"), "inputStringArray");
$client = new SoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));
$client->__soapCall("echoStringArray", array($param), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_base.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStringArray><inputStringArray SOAP-ENC:arrayType="xsd:string[2]" xsi:type="ns2:ArrayOfstring"><item xsi:type="xsd:string">good</item><item xsi:type="xsd:string">bad</item></inputStringArray></ns1:echoStringArray></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://soapinterop.org/xsd" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStringArrayResponse><outputStringArray SOAP-ENC:arrayType="xsd:string[2]" xsi:type="ns2:ArrayOfstring"><item xsi:type="xsd:string">good</item><item xsi:type="xsd:string">bad</item></outputStringArray></ns1:echoStringArrayResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
