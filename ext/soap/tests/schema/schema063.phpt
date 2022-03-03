--TEST--
SOAP XML Schema 63: standard unsignedLong type
--EXTENSIONS--
soap
xml
--INI--
precision=14
--FILE--
<?php
include "test_schema.inc";
$schema = '';
test_schema($schema,'type="xsd:unsignedLong"',0xffffffff);
echo "ok";
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam xsi:type="xsd:unsignedLong">4294967295</testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
%s(4294967295)
ok
