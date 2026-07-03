--TEST--
SoapServer constructor invalid type errors
--EXTENSIONS--
soap
--FILE--
<?php

$wsdl = 'irrelevant';

$options = [
    'uri' => 25,
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'actor' => 25,
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'classmap' => 25,
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'typemap' => 25,
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'features' => 'not an int',
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'cache_wsdl' => 'not an int',
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'send_errors' => 'not an int or bool',
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'trace' => 'not an int or bool',
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>WSDL</faultcode><faultstring>SOAP-ERROR: Parsing WSDL: Couldn't load from 'irrelevant' : failed to load external entity "irrelevant"
</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
