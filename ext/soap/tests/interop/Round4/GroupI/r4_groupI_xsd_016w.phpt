--TEST--
SOAP Interop Round4 GroupI XSD 016 (php/wsdl): echoDecimal
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/round4_groupI_xsd.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoDecimal(array("inputDecimal"=>"123456789.123456789"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupI_xsd.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/"><SOAP-ENV:Body><ns1:echoDecimal><ns1:inputDecimal>123456789.123456789</ns1:inputDecimal></ns1:echoDecimal></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/"><SOAP-ENV:Body><ns1:echoDecimalResponse><ns1:return>123456789.123456789</ns1:return></ns1:echoDecimalResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
