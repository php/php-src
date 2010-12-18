--TEST--
Bug #46760 (SoapClient doRequest fails when proxy is used)
--SKIPIF--
<?php require_once('skipif.inc'); 
if (!extension_loaded('sqlite')) die('skip sqlite extension not available');
?>
--FILE--
<?php
echo "blllllllllaaaaaaa\n";
$var475 = sqlite_factory("\x00");
$var147 = use_soap_error_handler();
$var477 = flock(false,false);dump($client->_proxy_port);
?>
--EXPECT--
blllllllllaaaaaaa
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Call to undefined function dump()</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
