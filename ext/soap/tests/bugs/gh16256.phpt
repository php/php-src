--TEST--
GH-16256 (Assertion failure in ext/soap/php_encoding.c:460)
--EXTENSIONS--
soap
--FILE--
<?php
$classmap = [
    "bogus",
];
$wsdl = __DIR__."/ext/soap/tests/bug41004.wsdl";
try {
    new SoapClient($wsdl, ["classmap" => $classmap]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    new SoapServer($wsdl, ["classmap" => $classmap]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
SoapClient::__construct(): 'classmap' option must be an associative array
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>SoapServer::__construct(): 'classmap' option must be an associative array</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
