--TEST--
Bug #40609 (Segfaults when using more than one SoapVar in a request)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
ini_set("soap.wsdl_cache_enabled", 0);

$c = new SoapClient(__DIR__."/bug40609.wsdl", array('trace' => 1, 'exceptions' => 0));

$c->update(array('symbol' => new SoapVar("<symbol>MSFT</symbol>", XSD_ANYXML),
                 'price' =>  new SoapVar("<price>1000</price>", XSD_ANYXML)));
echo $c->__getLastRequest();
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://quickstart.samples/xsd"><SOAP-ENV:Body><ns1:update><symbol>MSFT</symbol><price>1000</price></ns1:update></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
