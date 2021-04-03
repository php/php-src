--TEST--
SOAP XML Schema 70: Attribute with default value (attributeGroup)
--EXTENSIONS--
soap
xml
--FILE--
<?php
include "test_schema.inc";
$schema = <<<EOF
    <complexType name="testType">
        <attribute name="str" type="string"/>
        <attributeGroup ref="tns:int_group"/>
    </complexType>
    <attributeGroup name="int_group">
        <attribute name="int" type="int" default="5"/>
    </attributeGroup>
EOF;
test_schema($schema,'type="tns:testType"',(object)array("str"=>"str"));
echo "ok";
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam str="str" xsi:type="ns1:testType"/></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
object(stdClass)#%d (2) {
  ["str"]=>
  string(3) "str"
  ["int"]=>
  int(5)
}
ok
