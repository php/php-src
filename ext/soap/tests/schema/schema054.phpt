--TEST--
SOAP XML Schema 54: Apache Map
--EXTENSIONS--
soap
xml
--INI--
precision=14
--FILE--
<?php
include "test_schema.inc";
$schema = '';
test_schema($schema,'type="apache:Map" xmlns:apache="http://xml.apache.org/xml-soap"',array('a'=>123,'b'=>123.5));
echo "ok";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:ns2="http://xml.apache.org/xml-soap" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam xsi:type="ns2:Map"><item><key xsi:type="xsd:string">a</key><value xsi:type="xsd:int">123</value></item><item><key xsi:type="xsd:string">b</key><value xsi:type="xsd:float">123.5</value></item></testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
array(2) {
  ["a"]=>
  int(123)
  ["b"]=>
  float(123.5)
}
ok
