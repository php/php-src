--TEST--
SOAP XML Schema 62: NULL with attributes
--EXTENSIONS--
soap
xml
--FILE--
<?php
include "test_schema.inc";
$schema = <<<EOF
    <complexType name="testType">
        <simpleContent>
            <restriction base="int">
                <attribute name="int" type="int"/>
            </restriction>
        </simpleContent>
    </complexType>
EOF;
test_schema($schema,'type="tns:testType"',(object)array("_"=>NULL,"int"=>123.5));
echo "ok";
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam xsi:nil="true" int="123" xsi:type="ns1:testType"/></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
object(stdClass)#%d (2) {
  ["_"]=>
  NULL
  ["int"]=>
  int(123)
}
ok
