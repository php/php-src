--TEST--
SoapServer constructor with an invalid encoding option
--EXTENSIONS--
soap
--FILE--
<?php

class ExtendedSoapServer extends SoapServer {}

$wsdl = 'irrelevant';
$options = [
    'soap_version' => 1111,
];
try {
    $client = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $client = new ExtendedSoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}


?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>SoapServer::__construct(): 'soap_version' option must be SOAP_1_1 or SOAP_1_2</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
