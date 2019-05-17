--TEST--
Bug #77410: Segmentation Fault when executing method with an empty parameter
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$client = new class(__DIR__ . '/bug77410.wsdl', [
    'cache_wsdl' => WSDL_CACHE_NONE,
    'trace' => 1,
]) extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $one_way = 0) {
        echo $request, "\n";
        return ''; 
    }
};

$client->MyMethod([
    'parameter' => [],
]);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:test"><SOAP-ENV:Body><ns1:MyMethodRequest><parameter/></ns1:MyMethodRequest></SOAP-ENV:Body></SOAP-ENV:Envelope>
