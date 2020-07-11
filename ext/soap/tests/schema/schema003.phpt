--TEST--
SOAP XML Schema 3: simpleType/restriction (reference to type, that is not defined yet)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
include "test_schema.inc";
$schema = <<<EOF
    <simpleType name="testType">
        <restriction base="tns:testType2"/>
    </simpleType>
    <simpleType name="testType2">
        <restriction base="xsd:int"/>
    </simpleType>
EOF;
test_schema($schema,'type="tns:testType"',123.5);
echo "ok";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam xsi:type="ns1:testType">123</testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
int(123)
ok
