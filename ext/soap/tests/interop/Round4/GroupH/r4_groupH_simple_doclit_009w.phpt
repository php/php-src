--TEST--
SOAP Interop Round4 GroupH Simple Doc Lit 009 (php/wsdl): echoMultipleFaults2(2)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(dirname(__FILE__)."/round4_groupH_simple_doclit.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoMultipleFaults2(array("whichFault" => 2,
                                   "param1" => "Hello world",
                                   "param2" => 12.345,
                                   "param3" => array("one","two","three")));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupH_simple_doclit.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/types/requestresponse" xmlns:ns2="http://soapinterop.org/types"><SOAP-ENV:Body><ns1:echoMultipleFaults2Request><ns1:whichFault>2</ns1:whichFault><ns1:param1>Hello world</ns1:param1><ns1:param2>12.345</ns1:param2><ns1:param3><ns2:value>one</ns2:value><ns2:value>two</ns2:value><ns2:value>three</ns2:value></ns1:param3></ns1:echoMultipleFaults2Request></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/types/part"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Fault in response to 'echoMultipleFaults2'.</faultstring><detail><ns1:StringPart>Hello world</ns1:StringPart></detail></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
