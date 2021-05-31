--TEST--
SOAP XML Schema 82: SOAP 1.1 Array with SOAP_USE_XSI_ARRAY_TYPE (second way)
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
test_schema($schema,'type="tns:testType"',array(123,123.5),"rpc","encoded",'',SOAP_USE_XSI_ARRAY_TYPE);
echo "ok";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam SOAP-ENC:arrayType="xsd:int[2]" xsi:type="SOAP-ENC:Array"><x_item xsi:type="xsd:int">123</x_item><x_item xsi:type="xsd:int">123</x_item></testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
array(2) {
  [0]=>
  int(123)
  [1]=>
  int(123)
}
ok
