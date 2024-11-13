--TEST--
Bug #70388 (SOAP serialize_function_call() type confusion / RCE)
--EXTENSIONS--
soap
--FILE--
<?php

#[AllowDynamicProperties]
class MySoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $one_way = 0): string {
        echo $request, "\n";
        return '';
    }
}
$dummy = unserialize('O:12:"MySoapClient":3:{s:3:"uri";s:1:"X";s:8:"location";s:22:"http://localhost/a.xml";s:17:"__default_headers";a:1:{i:1;s:1337:"'.str_repeat("X", 1337).'";}}');
$dummy->notexisting();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="X" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Header/><SOAP-ENV:Body><ns1:notexisting/></SOAP-ENV:Body></SOAP-ENV:Envelope>
