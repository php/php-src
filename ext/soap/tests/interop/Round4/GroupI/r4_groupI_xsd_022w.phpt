--TEST--
SOAP Interop Round4 GroupI XSD 022 (php/wsdl): echoSimpleTypesAsComplexType(minOccurs=0)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(dirname(__FILE__)."/round4_groupI_xsd.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoSimpleTypesAsComplexType(array("inputInteger"=>34,
                                            "inputFloat"=>12.345));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupI_xsd.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/"><SOAP-ENV:Body><ns1:echoSimpleTypesAsComplexType><ns1:inputInteger>34</ns1:inputInteger><ns1:inputFloat>12.345</ns1:inputFloat></ns1:echoSimpleTypesAsComplexType></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd" xmlns:ns2="http://soapinterop.org/"><SOAP-ENV:Body><ns2:echoSimpleTypesAsComplexTypeResponse><ns2:return><ns1:varInt>34</ns1:varInt><ns1:varFloat>12.345</ns1:varFloat></ns2:return></ns2:echoSimpleTypesAsComplexTypeResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
