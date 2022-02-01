--TEST--
SOAP CLIENT WSS: SOAP 1.1 / WSS 1.1 SHA512 with additional header echo
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
include __DIR__.DIRECTORY_SEPARATOR."wss-test.inc";

$client = new LocalSoapClient(__DIR__.DIRECTORY_SEPARATOR."wss-test.wsdl");

$client->__setWSS(array(
	"random_id"			=> false,
	"digest_method"		=> SOAP_WSS_DIGEST_METHOD_SHA512,
	"wss_version"		=> SOAP_WSS_VERSION_1_1,
	"x509_binsectoken"	=> "TEST CERTIFICATE",
	"signfunc"			=> function($data) {
		return "TEST SIGNATURE";
	}
));

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
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://localhost/" xmlns:wsse11="http://docs.oasis-open.org/wss/oasis-wss-wssecurity-secext-1.1.xsd" xmlns:ds="http://www.w3.org/2000/09/xmldsig#" xmlns:wsu="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd" xmlns:ns2="http://localhost/header/">
%w<SOAP-ENV:Header>
%w<wsse11:Security SOAP-ENV:mustUnderstand="1">
%w<wsse11:BinarySecurityToken EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary" ValueType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-x509-token-profile-1.0#X509v3" wsu:Id="TokenID-1">%s</wsse11:BinarySecurityToken>
%w<ds:Signature>
%w<ds:SignedInfo>
%w<ds:CanonicalizationMethod Algorithm="http://www.w3.org/2001/10/xml-exc-c14n#"/>
%w<ds:SignatureMethod Algorithm="http://www.w3.org/2000/09/xmldsig#rsa-sha1"/>
%w<ds:Reference URI="#BodyID-1">
%w<ds:Transforms>
%w<ds:Transform Algorithm="http://www.w3.org/2001/10/xml-exc-c14n#"/>
%w</ds:Transforms>
%w<ds:DigestMethod Algorithm="http://www.w3.org/2001/04/xmlenc#sha512"/>
%w<ds:DigestValue>%s</ds:DigestValue>
%w</ds:Reference>
%w</ds:SignedInfo>
%w<ds:SignatureValue>%s</ds:SignatureValue>
%w<ds:KeyInfo>
%w<wsse11:SecurityTokenReference>
%w<wsse11:Reference URI="#TokenID-1" ValueType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-x509-token-profile-1.0#X509v3"/>
%w</wsse11:SecurityTokenReference>
%w</ds:KeyInfo>
%w</ds:Signature>
%w</wsse11:Security>
%w<ns2:TestHeader1>Hello</ns2:TestHeader1>
%w<ns2:TestHeader2>World</ns2:TestHeader2>
%w</SOAP-ENV:Header>
%w<SOAP-ENV:Body wsu:Id="BodyID-1">
%w<ns1:testFunctionIn>
%w<ns1:Param1>foo</ns1:Param1>
%w<ns1:Param2>bar</ns1:Param2>
%w</ns1:testFunctionIn>
%w</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
