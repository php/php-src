--TEST--
SOAP XML Schema 72: SOAP 1.1 Array (document style, element with inline type)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
include "test_schema.inc";
$schema = <<<EOF
	<element name="testElement">
	<complexType name="testType">
		<complexContent>
			<restriction base="SOAP-ENC:Array">
  	    <attribute ref="SOAP-ENC:arrayType" wsdl:arrayType="int[]"/>
    	</restriction>
    </complexContent>
	</complexType>
	</element>
EOF;
test_schema($schema,'element="tns:testElement"',array(123,123.5),'document','literal');
echo "ok";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:ns1="http://test-uri/"><SOAP-ENV:Body><ns1:testElement><xsd:int>123</xsd:int><xsd:int>123</xsd:int></ns1:testElement></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
