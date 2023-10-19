--TEST--
Segfault and assertion failure with refcounted props and arrays
--INI--
soap.wsdl_cache_enabled=0
--EXTENSIONS--
soap
--FILE--
<?php
class TestSoapClient extends SoapClient {
  function __doRequest($request, $location, $action, $version, $one_way = false): ?string {
        return <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://schemas.nothing.com" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body>
<ns1:dotest2Response><res xsi:type="SOAP-ENC:Struct">
    <a xsi:type="xsd:string">Hello</a>
    <b xsi:type="xsd:string">World</b>
</res>
</ns1:dotest2Response></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;
    }
}

trait A {
    public $a = [self::class . 'a'];
    public $b = self::class . 'b';
}

class DummyClass {
    use A;
}

$client = new TestSoapClient(__DIR__."/../classmap.wsdl", ['classmap' => ['Struct' => 'DummyClass']]);
var_dump($client->dotest2("???"));
?>
--EXPECT--
object(DummyClass)#2 (2) {
  ["a"]=>
  array(2) {
    [0]=>
    string(11) "DummyClassa"
    [1]=>
    string(5) "Hello"
  }
  ["b"]=>
  array(2) {
    [0]=>
    string(11) "DummyClassb"
    [1]=>
    string(5) "World"
  }
}
