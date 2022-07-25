--TEST--
SOAP CLIENT WSS: Only additional headers, no WSS headers echo
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
include __DIR__.DIRECTORY_SEPARATOR."wss-test.inc";

$client = new LocalSoapClient(__DIR__.DIRECTORY_SEPARATOR."wss-test.wsdl");

$client->__setSoapHeaders(array(
	new SoapHeader(
		'http://localhost/header/',
		'TestHeader1',
		'Hello'
	),
	new SoapHeader(
		'http://localhost/header/',
		'TestHeader2',
		'World'
	),
));

$client->testFunction(array(
	"Param1" => "foo",
	"Param2" => "bar"
));
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://localhost/" xmlns:ns2="http://localhost/header/">
%w<SOAP-ENV:Header>
%w<ns2:TestHeader1>Hello</ns2:TestHeader1>
%w<ns2:TestHeader2>World</ns2:TestHeader2>
%w</SOAP-ENV:Header>
%w<SOAP-ENV:Body>
%w<ns1:testFunctionIn>
%w<ns1:Param1>foo</ns1:Param1>
%w<ns1:Param2>bar</ns1:Param2>
%w</ns1:testFunctionIn>
%w</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
