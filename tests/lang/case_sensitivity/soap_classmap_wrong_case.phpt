--TEST--
SoapServer classmap with wrong-case class name throws Error
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$GLOBALS['HTTP_RAW_POST_DATA'] = '
<env:Envelope xmlns:env="http://schemas.xmlsoap.org/soap/envelope/"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xsd="http://www.w3.org/2001/XMLSchema"
    xmlns:enc="http://schemas.xmlsoap.org/soap/encoding/"
    xmlns:ns1="http://schemas.nothing.com"
>
  <env:Body>
    <dotest>
      <book xsi:type="ns1:book">
        <a xsi:type="xsd:string">Hello</a>
        <b xsi:type="xsd:string">World</b>
      </book>
    </dotest>
  </env:Body>
  <env:Header/>
</env:Envelope>';

class Book {
    public string $a = '';
    public string $b = '';
}

class TestService {
    public function dotest(Book $book): string {
        return get_class($book);
    }
}

$server = new SoapServer(__DIR__ . "/../../../ext/soap/tests/classmap.wsdl", [
    'actor'    => 'http://schema.nothing.com',
    'classmap' => ['book' => 'BOOK'],
]);
$server->setClass('TestService');
$server->handle($GLOBALS['HTTP_RAW_POST_DATA']);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Class "BOOK" not found</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
