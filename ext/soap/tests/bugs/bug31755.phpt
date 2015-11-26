--TEST--
Bug #31422 (No Error-Logging on SoapServer-Side)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$client=new SOAPClient(null, array('location' => 'http://localhost',
'uri' => 'myNS', 'exceptions' => false, 'trace' => true));

$header = new SOAPHeader(null, 'foo', 'bar');

$response= $client->__call('function', array(), null, $header);

print $client->__getLastRequest();
?>
--EXPECTF--
Warning: SoapHeader::SoapHeader(): Invalid namespace in %s on line %d
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="myNS" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Header/><SOAP-ENV:Body><ns1:function/></SOAP-ENV:Body></SOAP-ENV:Envelope>
