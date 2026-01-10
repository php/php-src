--TEST--
GH-20011 (Array of SoapVar of unknown type causes crash)
--EXTENSIONS--
soap
--FILE--
<?php
class TestSoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $one_way = false, ?string $uriParserClass = null): ?string {
        die($request);
    }
}

$array = [new SoapVar('test string', NULL)];

$client = new TestSoapClient(NULL, ['location' => 'test://', 'uri' => 'http://soapinterop.org/']);
$client->echoStringArray($array);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStringArray><param0 SOAP-ENC:arrayType="xsd:string[1]" xsi:type="SOAP-ENC:Array"><item xsi:type="xsd:string">test string</item></param0></ns1:echoStringArray></SOAP-ENV:Body></SOAP-ENV:Envelope>
