--TEST--
GH-15711 (SoapClient can't convert BackedEnum to scalar value)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

enum TestBackedEnum: string
{
    case First = 'BackingValue1';
    case Second = 'BackingValue2';
    case Third = 'BackingValue3';
    case Fourth = 'BackingValue4';
    case Fifth = 'BackingValue5';
}

enum TestNonBackedEnum
{
    case First;
}

class TestSoapClient extends SoapClient {
    function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
        echo $request;
    }
}

$client = new TestSoapClient('ext/soap/tests/gh15711.wsdl', ['classmap' => ['book' => 'book']]);

echo "--- Test with backed enum ---\n";

$book = new stdClass();
$book->base64 = TestBackedEnum::First;
$book->string = TestBackedEnum::Second;
$book->any = TestBackedEnum::Third;
$book->hexbin = TestBackedEnum::Fourth;
$book->nmtokens = TestBackedEnum::Fifth;

try {
    $client->dotest($book);
} catch (Throwable) {}

echo "--- Test with non-backed enum ---\n";

$book = new stdClass();
$book->base64 = TestNonBackedEnum::First;
$book->string = TestNonBackedEnum::First;
$book->any = TestNonBackedEnum::First;
$book->hexbin = TestNonBackedEnum::First;
$book->nmtokens = TestNonBackedEnum::First;

try {
    $client->dotest($book);
} catch (ValueError $e) {
    echo "ValueError: ", $e->getMessage(), "\n";
}

?>
--EXPECT--
--- Test with backed enum ---
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://schemas.nothing.com" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:dotest><dotestReturn xsi:type="ns1:book"><base64 xsi:type="xsd:base64Binary">QmFja2luZ1ZhbHVlMQ==</base64><string xsi:type="xsd:string">BackingValue2</string><any xsi:type="xsd:any"><name xsi:type="xsd:string">Third</name><value xsi:type="xsd:string">BackingValue3</value></any><hexbin xsi:type="xsd:hexBinary">4261636B696E6756616C756534</hexbin><nmtokens>BackingValue5</nmtokens></dotestReturn></ns1:dotest></SOAP-ENV:Body></SOAP-ENV:Envelope>
--- Test with non-backed enum ---
ValueError: Non-backed enums have no default serialization
