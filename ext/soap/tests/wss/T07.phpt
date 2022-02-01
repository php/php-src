--TEST--
SOAP CLIENT WSS: SOAP 1.1 / WSS 1.0 SHA1 with user defined function and without binsectoken echo
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
include __DIR__.DIRECTORY_SEPARATOR."wss-test.inc";

function __wss_test_signfunc($data) {
	return "TEST SIGNATURE";
}

$client = new LocalSoapClient(__DIR__.DIRECTORY_SEPARATOR."wss-test.wsdl");

$client->__setWSS(array(
	"random_id"	=> false,
	"signfunc"	=> "__wss_test_signfunc"
));

$client->testFunction(array(
	"Param1" => "foo",
	"Param2" => "bar"
));
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://localhost/" xmlns:wsse="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd" xmlns:ds="http://www.w3.org/2000/09/xmldsig#" xmlns:wsu="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">
%w<SOAP-ENV:Header>
%w<wsse:Security SOAP-ENV:mustUnderstand="1">
%w<ds:Signature>
%w<ds:SignedInfo>
%w<ds:CanonicalizationMethod Algorithm="http://www.w3.org/2001/10/xml-exc-c14n#"/>
%w<ds:SignatureMethod Algorithm="http://www.w3.org/2000/09/xmldsig#rsa-sha1"/>
%w<ds:Reference URI="#BodyID-1">
%w<ds:Transforms>
%w<ds:Transform Algorithm="http://www.w3.org/2001/10/xml-exc-c14n#"/>
%w</ds:Transforms>
%w<ds:DigestMethod Algorithm="http://www.w3.org/2000/09/xmldsig#sha1"/>
%w<ds:DigestValue>OaQiUDsEKrCjytgTUqF4CLsB9jo=</ds:DigestValue>
%w</ds:Reference>
%w</ds:SignedInfo>
%w<ds:SignatureValue>VEVTVCBTSUdOQVRVUkU=</ds:SignatureValue>
%w</ds:Signature>
%w</wsse:Security>
%w</SOAP-ENV:Header>
%w<SOAP-ENV:Body wsu:Id="BodyID-1">
%w<ns1:testFunctionIn>
%w<ns1:Param1>foo</ns1:Param1>
%w<ns1:Param2>bar</ns1:Param2>
%w</ns1:testFunctionIn>
%w</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
