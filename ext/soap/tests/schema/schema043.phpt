--TEST--
SOAP XML Schema 43: Extension of simple type (2)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
include "test_schema.inc";
$schema = <<<EOF
	<complexType name="testType2">
		<simpleContent>
			<extension base="int">
				<attribute name="int" type="int"/>
			</extension>
		</simpleContent>
	</complexType>
	<complexType name="testType">
		<simpleContent>
			<extension base="tns:testType2">
				<attribute name="int2" type="int"/>
			</extension>
		</simpleContent>
	</complexType>
EOF;
test_schema($schema,'type="tns:testType"',(object)array("_"=>123.5,"int"=>123.5,"int2"=>123.5));
echo "ok";
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam xsi:type="ns1:testType" int="123" int2="123">123</testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
object(stdClass)#%d (3) {
  ["_"]=>
  int(123)
  ["int"]=>
  int(123)
  ["int2"]=>
  int(123)
}
ok
