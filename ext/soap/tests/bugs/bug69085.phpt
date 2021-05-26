--TEST--
Bug #69085 (SoapClient's __call() type confusion through unserialize())
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

class MySoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $one_way = 0): string {
        echo $request, "\n";
        return '';
    }
}
$dummy = unserialize('O:12:"MySoapClient":5:{s:3:"uri";s:1:"a";s:8:"location";s:22:"http://localhost/a.xml";s:17:"__default_headers";i:1337;s:15:"__last_response";s:1:"a";s:5:"trace";s:1:"x";}');
$dummy->whatever();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="a" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:whatever/></SOAP-ENV:Body></SOAP-ENV:Envelope>
