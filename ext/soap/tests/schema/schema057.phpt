--TEST--
SOAP XML Schema 57: SOAP 1.1 Array (second way, literal encoding)
--EXTENSIONS--
soap
xml
--FILE--
<?php
include "test_schema.inc";
$schema = <<<EOF
    <complexType name="testType">
        <complexContent>
            <restriction base="SOAP-ENC:Array">
                <all>
                    <element name="x_item" type="int" maxOccurs="unbounded"/>
            </all>
        </restriction>
    </complexContent>
    </complexType>
EOF;
test_schema($schema,'type="tns:testType"',array(123,123.5),'rpc','literal');
echo "ok";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><SOAP-ENV:Body><ns1:test><testParam><x_item>123</x_item><x_item>123</x_item></testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
array(2) {
  [0]=>
  int(123)
  [1]=>
  int(123)
}
ok
