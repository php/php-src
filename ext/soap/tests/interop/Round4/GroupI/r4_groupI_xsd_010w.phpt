--TEST--
SOAP Interop Round4 GroupI XSD 010 (php/wsdl): echoIntegerMultiOccurs
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/round4_groupI_xsd.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoIntegerMultiOccurs(array("inputIntegerMultiOccurs"=>array(22,29,36)));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupI_xsd.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/"><SOAP-ENV:Body><ns1:echoIntegerMultiOccurs><ns1:inputIntegerMultiOccurs><ns1:int>22</ns1:int><ns1:int>29</ns1:int><ns1:int>36</ns1:int></ns1:inputIntegerMultiOccurs></ns1:echoIntegerMultiOccurs></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/"><SOAP-ENV:Body><ns1:echoIntegerMultiOccursResponse><ns1:return>22</ns1:return><ns1:return>29</ns1:return><ns1:return>36</ns1:return></ns1:echoIntegerMultiOccursResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
