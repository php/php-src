--TEST--
SOAP Interop Round3 GroupD Compound1 002 (php/wsdl): echoDocument
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/round3_groupD_compound1.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoDocument("Test Document Here");
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round3_groupD_compound1.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd"><SOAP-ENV:Body><ns1:x_Document>Test Document Here</ns1:x_Document></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd"><SOAP-ENV:Body><ns1:result_Document>Test Document Here</ns1:result_Document></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
