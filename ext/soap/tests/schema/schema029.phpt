--TEST--
SOAP XML Schema 29: SOAP 1.2 Multidimensional array (second way)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
include "test_schema.inc";
$schema = <<<EOF
    <complexType name="testType">
        <complexContent>
            <restriction base="enc12:Array" xmlns:enc12="http://www.w3.org/2003/05/soap-encoding">
                <all>
                    <element name="x_item" type="int" maxOccurs="unbounded"/>
            </all>
        <attribute ref="enc12:arraySize" wsdl:arraySize="* 1"/>
        </restriction>
    </complexContent>
    </complexType>
EOF;
test_schema($schema,'type="tns:testType"',array(array(123),array(123.5)));
echo "ok";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam SOAP-ENC:arrayType="xsd:int[2,1]" xsi:type="ns1:testType"><x_item xsi:type="xsd:int">123</x_item><x_item xsi:type="xsd:int">123</x_item></testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(123)
  }
  [1]=>
  array(1) {
    [0]=>
    int(123)
  }
}
ok
