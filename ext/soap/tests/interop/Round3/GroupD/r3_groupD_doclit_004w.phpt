--TEST--
SOAP Interop Round3 GroupD Doc Lit 004 (php/wsdl): echoVoid
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/round3_groupD_doclit.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoVoid();
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round3_groupD_doclit.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body/></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body/></SOAP-ENV:Envelope>
ok
