--TEST--
Bug #42488 (SoapServer reports an encoding error and the error itself breaks)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$request = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="test:\" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:getBadUTF/></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;
$soap = new SoapServer(NULL, array('uri'=>'test://'));
function getBadUTF(){
    return "stuff\x93thing";
}
$soap->addFunction('getBadUTF');
$soap->handle($request);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>SOAP-ERROR: Encoding: string 'stuff\x93...' is not a valid utf-8 string</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>