--TEST--
Bug #69668 (SOAP: special XML characters in namespace URIs not encoded)
--EXTENSIONS--
soap
--FILE--
<?php
class MySoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
        echo $request, PHP_EOL;
        return '';
    }
}

$client = new MySoapClient(__DIR__ . '/bug69668.wsdl', [
    'trace'      => true,
    'exceptions' => true,
    'cache_wsdl' => WSDL_CACHE_NONE,
]);

$client->test();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://localhost/69668.php?a=a&amp;b=b" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test/></SOAP-ENV:Body></SOAP-ENV:Envelope>
