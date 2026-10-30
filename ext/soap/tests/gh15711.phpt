--TEST--
GH-15711 (SoapClient can't convert BackedEnum to scalar value)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

enum StringBackedEnum: string
{
    case First = 'BackingValue1';
    case Second = 'BackingValue2';
    case Third = 'BackingValue3';
    case Fourth = 'BackingValue4';
    case Fifth = 'BackingValue5';
}

enum IntBackedEnum: int
{
    case First = 1;
    case Second = 2;
}

enum NonBackedEnum
{
    case First;
}

class TestSoapClient extends SoapClient {
    function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
        echo $request;
    }
}

$client = new TestSoapClient(__DIR__ . '/gh15711.wsdl', ['classmap' => ['book' => 'book']]);

echo "--- Test with backed enum ---\n";

$book = new stdClass();
$book->base64 = StringBackedEnum::First;
$book->string = StringBackedEnum::Second;
$book->any = StringBackedEnum::Third;
$book->hexbin = StringBackedEnum::Fourth;
$book->nmtokens = StringBackedEnum::Fifth;
$book->integer = IntBackedEnum::First;
$book->short = IntBackedEnum::Second;

try {
    $client->dotest($book);
} catch (Throwable) {}

echo "--- Test with non-backed enum ---\n";

$book = new stdClass();
$book->base64 = NonBackedEnum::First;
$book->string = NonBackedEnum::First;
$book->any = NonBackedEnum::First;
$book->hexbin = NonBackedEnum::First;
$book->nmtokens = NonBackedEnum::First;
$book->integer = NonBackedEnum::First;
$book->short = NonBackedEnum::First;

try {
    $client->dotest($book);
} catch (ValueError $e) {
    echo "ValueError: ", $e->getMessage(), "\n";
}

echo "--- Test with mismatched enum backing type ---\n";

$book->integer = StringBackedEnum::First;
$book->short = StringBackedEnum::First;
try {
    $client->dotest($book);
} catch (ValueError $e) {
    echo "ValueError: ", $e->getMessage(), "\n";
}

?>
--EXPECT--
--- Test with backed enum ---
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://schemas.nothing.com" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:dotest><dotestReturn xsi:type="ns1:book"><base64 xsi:type="xsd:base64Binary">QmFja2luZ1ZhbHVlMQ==</base64><string xsi:type="xsd:string">BackingValue2</string><any xsi:type="xsd:any"><name xsi:type="xsd:string">Third</name><value xsi:type="xsd:string">BackingValue3</value></any><hexbin xsi:type="xsd:hexBinary">4261636B696E6756616C756534</hexbin><nmtokens>BackingValue5</nmtokens><integer xsi:type="xsd:integer">1</integer><short xsi:type="xsd:short">2</short></dotestReturn></ns1:dotest></SOAP-ENV:Body></SOAP-ENV:Envelope>
--- Test with non-backed enum ---
ValueError: Non-backed enums have no default serialization
--- Test with mismatched enum backing type ---
ValueError: String-backed enum cannot be serialized as int
