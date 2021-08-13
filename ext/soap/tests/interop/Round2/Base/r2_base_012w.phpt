--TEST--
SOAP Interop Round2 base 012 (php/wsdl): echoFloat
--EXTENSIONS--
soap
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/round2_base.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoFloat(342.23);
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_base.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoFloat><inputFloat xsi:type="xsd:float">342.23</inputFloat></ns1:echoFloat></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoFloatResponse><outputFloat xsi:type="xsd:float">342.23</outputFloat></ns1:echoFloatResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
