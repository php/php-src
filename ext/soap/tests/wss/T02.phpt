--TEST--
SOAP CLIENT WSS: Remove WSS headers echo
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
include __DIR__.DIRECTORY_SEPARATOR."wss-test.inc";

$client = new LocalSoapClient(__DIR__.DIRECTORY_SEPARATOR."wss-test.wsdl");

$client->__setWSS(array(
	"x509_binsectoken"	=> "TEST CERTIFICATE",
	"signfunc"			=> function($data) {
		return "TEST SIGNATURE";
	}
));

$client->__setWSS();

$client->testFunction(array(
	"Param1" => "foo",
	"Param2" => "bar"
));
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://localhost/">
%w<SOAP-ENV:Body>
%w<ns1:testFunctionIn>
%w<ns1:Param1>foo</ns1:Param1>
%w<ns1:Param2>bar</ns1:Param2>
%w</ns1:testFunctionIn>
%w</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
