--TEST--
Bug #42214 (SoapServer sends clients internal PHP errors)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$request = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://localhost/server.php" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test/></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;

function test() {
	$a = $b;
	obvious_error(); // will cause an error
}

$server = new SoapServer(NULL, array('uri' =>'http://localhost/server.php',
	'send_errors'=>0));
$server->addFunction('test');
$server->handle($request);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Internal Error</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
