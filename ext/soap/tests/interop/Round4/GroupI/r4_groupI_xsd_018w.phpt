--TEST--
SOAP Interop Round4 GroupI XSD 018 (php/wsdl): echoHexBinary
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/round4_groupI_xsd.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoHexBinary(array("inputHexBinary"=>"\x80\xFF\x00\x01\x7F"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupI_xsd.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/"><SOAP-ENV:Body><ns1:echoHexBinary><ns1:inputHexBinary>80FF00017F</ns1:inputHexBinary></ns1:echoHexBinary></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/"><SOAP-ENV:Body><ns1:echoHexBinaryResponse><ns1:return>80FF00017F</ns1:return></ns1:echoHexBinaryResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
