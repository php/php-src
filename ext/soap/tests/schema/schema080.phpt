--TEST--
SOAP XML Schema 80: Element form qualified/unqualified (elementFormDefault - default)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
include "test_schema.inc";
$schema = <<<EOF
	<complexType name="testType">
		<sequence>
			<element name="int1" type="int"/>
			<element name="int2" type="int" form="qualified"/>
			<element name="int3" type="int" form="unqualified"/>
		</sequence>
	</complexType>
EOF;

test_schema($schema,'type="tns:testType"',(object)array("int1"=>1.1,"int2"=>2.2,"int3"=>3.3), "rpc", "literal");
echo "ok";
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/"><SOAP-ENV:Body><ns1:test><testParam><int1>1</int1><ns1:int2>2</ns1:int2><int3>3</int3></testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
object(stdClass)#%d (3) {
  ["int1"]=>
  int(1)
  ["int2"]=>
  int(2)
  ["int3"]=>
  int(3)
}
ok
