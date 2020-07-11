--TEST--
SOAP Interop Round4 GroupI XSD 031 (php/wsdl): echoVoidSoapHeader(2)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$hdr = new SoapHeader("http://soapinterop.org/","echoMeStringRequest", array(), 1);
$client = new SoapClient(__DIR__."/round4_groupI_xsd.wsdl",array("trace"=>1,"exceptions"=>0));
$client->__soapCall("echoVoidSoapHeader",array(),null,$hdr);
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupI_xsd.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org" xmlns:ns2="http://soapinterop.org/"><SOAP-ENV:Header><ns2:echoMeStringRequest SOAP-ENV:mustUnderstand="1"/></SOAP-ENV:Header><SOAP-ENV:Body><ns1:echoVoidSoapHeader/></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/"><SOAP-ENV:Header><ns1:echoMeStringResponse/></SOAP-ENV:Header><SOAP-ENV:Body><ns1:echoVoidSoapHeaderResponse/></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
