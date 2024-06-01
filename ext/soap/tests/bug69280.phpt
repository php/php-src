--TEST--
Bug #69280 (SoapClient classmap doesn't support fully qualified class name)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--CREDITS--
champetier dot etienne at gmail dot com
--FILE--
<?php
abstract class AbstractClass {
	public $prop;
}

class RealClass1 extends AbstractClass {
	public $prop1;
}

class TestWS extends \SoapClient {
	public function TestMethod($parameters) {
		return $this->__soapCall('TestMethod', [$parameters], [
				'uri' => 'http://tempuri.org/',
				'soapaction' => ''
			]
		);
	}

	public function __doRequest(string $request, string $location, string $action, int $version, bool $oneWay = false): ?string {
		die($request);
	}
}

$a = new TestWS(__DIR__ . '/bug69280.wsdl', ['classmap' => [
	'AbstractClass' => '\AbstractClass',
	'RealClass1' => '\RealClass1',
]]);
$r1 = new \RealClass1();
$r1->prop = "prop";
$r1->prop1 = "prop1";
$a->TestMethod($r1);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://tempuri.org/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><SOAP-ENV:Body><parameters xsi:type="ns1:RealClass1"><ns1:prop>prop</ns1:prop><ns1:prop1>prop1</ns1:prop1></parameters></SOAP-ENV:Body></SOAP-ENV:Envelope>
