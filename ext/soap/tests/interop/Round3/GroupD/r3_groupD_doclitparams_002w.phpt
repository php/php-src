--TEST--
SOAP Interop Round3 GroupD Doc Lit Parameters 002 (php/wsdl): echoStringArray
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/round3_groupD_doclitparams.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoStringArray(array("param0"=>array("one","two","three")));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round3_groupD_doclitparams.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd"><SOAP-ENV:Body><ns1:echoStringArray><ns1:param0><ns1:string>one</ns1:string><ns1:string>two</ns1:string><ns1:string>three</ns1:string></ns1:param0></ns1:echoStringArray></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd"><SOAP-ENV:Body><ns1:echoStringArrayResponse><ns1:return><ns1:string>one</ns1:string><ns1:string>two</ns1:string><ns1:string>three</ns1:string></ns1:return></ns1:echoStringArrayResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
