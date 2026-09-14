--TEST--
GH-22218 (SoapServer::handle() segfault on non-array/unset $_SERVER)
--EXTENSIONS--
soap
--CREDITS--
Rex-Reynolds
--SKIPIF--
<?php
if (php_sapi_name() == 'cli') echo 'skip needs request body (POST)';
?>
--POST--
<SOAP-ENV:Envelope
    xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">
    <SOAP-ENV:Body>
        <test/>
    </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
--FILE--
<?php
$_SERVER = 79;
$server = new SoapServer(null, ['uri' => 'http://test-uri']);
$server->handle();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Call to undefined function test()</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
