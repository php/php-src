--TEST--
SOAP Interop Round4 GroupH Simple Doc Lit 003 (php/wsdl): echoIntArrayFault
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(dirname(__FILE__)."/round4_groupH_simple_doclit.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoIntArrayFault(array(34,12.345));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupH_simple_doclit.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/types" xmlns:ns2="http://soapinterop.org/types/requestresponse"><SOAP-ENV:Body><ns2:echoIntArrayFaultRequest><ns1:value>34</ns1:value><ns1:value>12</ns1:value></ns2:echoIntArrayFaultRequest></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/types" xmlns:ns2="http://soapinterop.org/types/part"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Fault in response to 'echoIntArrayFault'.</faultstring><detail><ns2:ArrayOfIntPart><ns1:value>34</ns1:value><ns1:value>12</ns1:value></ns2:ArrayOfIntPart></detail></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
