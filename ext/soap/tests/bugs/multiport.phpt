--TEST--
Proper binding selection
--SKIPIF--
<?php require_once 'skipif.inc'; ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__.'/multiport.wsdl',
	array('trace' => true, 'exceptions' => false));
$response = $client->GetSessionId(array('userId'=>'user', 'password'=>'password'));
echo $client->__getLastRequest();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://www.reuters.com/"><SOAP-ENV:Body><ns1:GetSessionId><ns1:userId>user</ns1:userId><ns1:password>password</ns1:password></ns1:GetSessionId></SOAP-ENV:Body></SOAP-ENV:Envelope>
