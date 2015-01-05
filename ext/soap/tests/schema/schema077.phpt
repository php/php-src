--TEST--
SOAP XML Schema 77: Attributes form qualified/unqualified (attributeFormDefault - default)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
include "test_schema.inc";
$schema = <<<EOF
	<complexType name="testType">
		<attribute name="int1" type="int"/>
		<attribute name="int2" type="int" form="qualified"/>
		<attribute name="int3" type="int" form="unqualified"/>
	</complexType>
EOF;

test_schema($schema,'type="tns:testType"',(object)array("int1"=>1.1,"int2"=>2.2,"int3"=>3.3), "rpc", "encoded");
echo "ok";
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam int1="1" ns1:int2="2" int3="3" xsi:type="ns1:testType"/></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
object(stdClass)#%d (3) {
  ["int1"]=>
  int(1)
  ["int2"]=>
  int(2)
  ["int3"]=>
  int(3)
}
ok
