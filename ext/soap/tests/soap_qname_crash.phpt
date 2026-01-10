--TEST--
Test SoapClient with excessively large QName prefix in SoapVar
--EXTENSIONS--
soap
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip: 64-bit only");
?>
--INI--
memory_limit=6144M
--FILE--
<?php

class TestSoapClient extends SoapClient {
    public function __doRequest(
        $request,
        $location,
        $action,
        $version,
        $one_way = false,
        ?string $uriParserClass = null,
    ): ?string {
        die($request);
    }
}

$prefix = str_repeat("A", 2 * 1024 * 1024 * 1024);
$qname = "{$prefix}:tag";

echo "Attempting to create SoapVar with very large QName\n";

$var = new SoapVar("value", XSD_QNAME, null, null, $qname);

echo "Attempting encoding\n";

$options = [
    'location' => 'http://127.0.0.1/',
    'uri' => 'urn:dummy',
    'trace' => 1,
    'exceptions' => true,
];
$client = new TestSoapClient(null, $options);
$client->__soapCall("DummyFunction", [$var]);
?>
--EXPECT--
Attempting to create SoapVar with very large QName
Attempting encoding
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:dummy" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:DummyFunction><param0 xsi:type="xsd:QName">value</param0></ns1:DummyFunction></SOAP-ENV:Body></SOAP-ENV:Envelope>
