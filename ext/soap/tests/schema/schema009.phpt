--TEST--
SOAP XML Schema 9: simpleType/list (as string)
--EXTENSIONS--
soap
xml
--FILE--
<?php
include "test_schema.inc";
$schema = <<<EOF
    <simpleType name="testType">
        <list itemType="token"/>
    </simpleType>
EOF;
test_schema($schema,'type="tns:testType"',"one two");
echo "ok";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam xsi:type="ns1:testType">one two</testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
string(7) "one two"
ok
