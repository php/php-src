--TEST--
SOAP Interop Round4 GroupH SoapFault 004 (php/wsdl): echoMustUnderstandFault
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$hdr = new SoapHeader("http://soapinterop.org/wsdl", "UnknownHeaderRequest", "Hello World", 1);
$client = new SoapClient(dirname(__FILE__)."/round4_groupH_soapfault.wsdl",array("trace"=>1,"exceptions"=>0));
$client->__soapCall("echoVersionMismatchFault",array(), null, $hdr);
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupH_soapfault.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/wsdl" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Header><ns1:UnknownHeaderRequest SOAP-ENV:mustUnderstand="1">Hello World</ns1:UnknownHeaderRequest></SOAP-ENV:Header><SOAP-ENV:Body><ns1:echoVersionMismatchFault/></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:MustUnderstand</faultcode><faultstring>Header not understood</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
