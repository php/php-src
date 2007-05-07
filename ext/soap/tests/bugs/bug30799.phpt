--TEST--
Bug #30799 (SoapServer doesn't handle private or protected properties)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class foo {
	public    $a="a";
	private   $b="b";
	protected $c="c";
		
}

$x = new SoapClient(NULL,array("location"=>"test://",
                               "uri" => "test://",
                               "exceptions" => 0,
                               "trace" => 1 ));
$x->test(new foo());
echo $x->__getLastRequest();
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="test://" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><param0 xsi:type="SOAP-ENC:Struct"><a xsi:type="xsd:string">a</a><b xsi:type="xsd:string">b</b><c xsi:type="xsd:string">c</c></param0></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
