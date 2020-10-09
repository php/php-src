--TEST--
SOAP Interop Round3 GroupD Doc Lit 002 (php/wsdl): echoStringArray
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/round3_groupD_doclit.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoStringArray(array("one","two","three"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round3_groupD_doclit.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd"><SOAP-ENV:Body><ns1:echoStringArrayParam><ns1:string>one</ns1:string><ns1:string>two</ns1:string><ns1:string>three</ns1:string></ns1:echoStringArrayParam></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd"><SOAP-ENV:Body><ns1:echoStringArrayReturn><ns1:string>one</ns1:string><ns1:string>two</ns1:string><ns1:string>three</ns1:string></ns1:echoStringArrayReturn></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
