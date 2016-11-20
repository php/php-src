--TEST--
SOAP: SoapClient::__setHeaders array overrides previous headers
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$client = new SoapClient(null, [
    "location"      =>  "test://",
    "uri"           =>  "test://",
    "exceptions"    =>  false,
    "trace"         =>  true,
]);
$client->__setSoapHeaders(new \SoapHeader('ns', 'Header', ['something' => 1]));
$client->__setSoapHeaders(new \SoapHeader('ns', 'Header', ['something' => 2]));
$client->test();
echo $client->__getLastRequest();

$client = new SoapClient(null, [
    "location"      =>  "test://",
    "uri"           =>  "test://",
    "exceptions"    =>  false,
    "trace"         =>  true,
]);
$client->__setSoapHeaders([new \SoapHeader('ns', 'Header', ['something' => 1])]);
$client->__setSoapHeaders([new \SoapHeader('ns', 'Header', ['something' => 2])]);
$client->test();
echo $client->__getLastRequest();

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="test://" xmlns:ns2="ns" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Header><ns2:Header><item><key>something</key><value>2</value></item></ns2:Header></SOAP-ENV:Header><SOAP-ENV:Body><ns1:test/></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="test://" xmlns:ns2="ns" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Header><ns2:Header><item><key>something</key><value>2</value></item></ns2:Header></SOAP-ENV:Header><SOAP-ENV:Body><ns1:test/></SOAP-ENV:Body></SOAP-ENV:Envelope>
