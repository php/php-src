--TEST--
Bug #50997 (SOAP Error when trying to submit 2nd Element of a choice)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$soapClient = new SoapClient(__DIR__ . '/bug50997.wsdl', array('trace' => 1, 'exceptions'=>0));
$params = array('code'=>'foo');
$soapClient->newOperation($params);
echo $soapClient->__getLastRequest();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://www.example.org/service2/"><SOAP-ENV:Body><ns1:NewOperation><code>foo</code></ns1:NewOperation></SOAP-ENV:Body></SOAP-ENV:Envelope>
