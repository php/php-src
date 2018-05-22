--TEST--
Bug #48557 (Numeric string keys in Apache Hashmaps are not cast to integers)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);
ini_set("soap.wsdl_cache_enabled", 0);

function test($map) {
	var_dump($map, $map[1], $map[2]);die;
}

$y = new SoapServer(dirname(__FILE__) . '/bug48557.wsdl');
$y->addfunction("test");
$request = <<<XML
<?xml version="1.0"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:ns2="http://xml.apache.org/xml-soap" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
	<SOAP-ENV:Body>
		<ns1:test>
			<testParam xsi:type="ns2:Map">
				<item>
					<key xsi:type="xsd:int">1</key>
					<value xsi:type="xsd:int">123</value>
				</item>
				<item>
					<key xsi:type="xsd:int">-1000</key>
					<value xsi:type="xsd:string">123</value>
				</item>
				<item>
					<key xsi:type="xsd:string">2</key>
					<value xsi:type="xsd:float">123.5</value>
				</item>
				<item>
					<key xsi:type="xsd:string">-2000</key>
					<value xsi:type="xsd:float">123.5</value>
				</item>
				<item>
					<key xsi:type="xsd:string">011</key>
					<value xsi:type="xsd:float">123.5</value>
				</item>
				<item>
					<key xsi:type="xsd:int">012</key>
					<value xsi:type="xsd:float">123.5</value>
				</item>
			</testParam>
		</ns1:test>
	</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;

$y->handle($request);

?>
===DONE===
--EXPECTF--
array(6) {
  [1]=>
  int(123)
  [-1000]=>
  string(3) "123"
  [2]=>
  float(123.5)
  [-2000]=>
  float(123.5)
  ["011"]=>
  float(123.5)
  [12]=>
  float(123.5)
}
int(123)
float(123.5)
