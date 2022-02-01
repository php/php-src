--TEST--
SOAP CLIENT WSS: SOAP 1.2 / WSS 1.1 SHA256 with timestamp and random id echo
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
include __DIR__.DIRECTORY_SEPARATOR."wss-test.inc";

$client = new LocalSoapClient(__DIR__.DIRECTORY_SEPARATOR."wss-test.wsdl",array(
	"soap_version" => SOAP_1_2
));

$client->__setWSS(array(
	"add_timestamp"		=> true,
    "timestamp_expires"	=> 300,
    "digest_method"		=> SOAP_WSS_DIGEST_METHOD_SHA256,
    "wss_version"		=> SOAP_WSS_VERSION_1_1,
	"x509_binsectoken"	=> "TEST CERTIFICATE",
	"signfunc"			=> function($data) {
		return "TEST SIGNATURE";
	}
));

$client->testFunction(array(
	"Param1" => "foo",
	"Param2" => "bar"
));
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="http://localhost/" xmlns:wsse11="http://docs.oasis-open.org/wss/oasis-wss-wssecurity-secext-1.1.xsd" xmlns:ds="http://www.w3.org/2000/09/xmldsig#" xmlns:wsu="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">
%w<env:Header>
%w<wsse11:Security env:mustUnderstand="true">
%w<wsu:Timestamp wsu:Id="TimestampID-%d">
%w<wsu:Created>%r([0-9]{4})%r-%r([0-9]{2})%r-%r([0-9]{2})%rT%r([0-9]{2})%r:%r([0-9]{2})%r:%r([0-9]{2})%rZ</wsu:Created>
%w<wsu:Expires>%r([0-9]{4})%r-%r([0-9]{2})%r-%r([0-9]{2})%rT%r([0-9]{2})%r:%r([0-9]{2})%r:%r([0-9]{2})%rZ</wsu:Expires>
%w</wsu:Timestamp>
%w<wsse11:BinarySecurityToken EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary" ValueType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-x509-token-profile-1.0#X509v3" wsu:Id="TokenID-%d">%s</wsse11:BinarySecurityToken>
%w<ds:Signature>
%w<ds:SignedInfo>
%w<ds:CanonicalizationMethod Algorithm="http://www.w3.org/2001/10/xml-exc-c14n#"/>
%w<ds:SignatureMethod Algorithm="http://www.w3.org/2000/09/xmldsig#rsa-sha1"/>
%w<ds:Reference URI="#TimestampID-%d">
%w<ds:Transforms>
%w<ds:Transform Algorithm="http://www.w3.org/2001/10/xml-exc-c14n#"/>
%w</ds:Transforms>
%w<ds:DigestMethod Algorithm="http://www.w3.org/2001/04/xmlenc#sha256"/>
%w<ds:DigestValue>%s</ds:DigestValue>
%w</ds:Reference>
%w<ds:Reference URI="#BodyID-%d">
%w<ds:Transforms>
%w<ds:Transform Algorithm="http://www.w3.org/2001/10/xml-exc-c14n#"/>
%w</ds:Transforms>
%w<ds:DigestMethod Algorithm="http://www.w3.org/2001/04/xmlenc#sha256"/>
%w<ds:DigestValue>%s</ds:DigestValue>
%w</ds:Reference>
%w</ds:SignedInfo>
%w<ds:SignatureValue>%s</ds:SignatureValue>
%w<ds:KeyInfo>
%w<wsse11:SecurityTokenReference>
%w<wsse11:Reference URI="#TokenID-%d" ValueType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-x509-token-profile-1.0#X509v3"/>
%w</wsse11:SecurityTokenReference>
%w</ds:KeyInfo>
%w</ds:Signature>
%w</wsse11:Security>
%w</env:Header>
%w<env:Body wsu:Id="BodyID-%d">
%w<ns1:testFunctionIn>
%w<ns1:Param1>foo</ns1:Param1>
%w<ns1:Param2>bar</ns1:Param2>
%w</ns1:testFunctionIn>
%w</env:Body>
</env:Envelope>
