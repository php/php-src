--TEST--
SOAP Server 25: One-way SOAP headers encoding using WSDL
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class TestHeader1 extends SoapHeader {
    function __construct($data) {
        parent::__construct("http://testuri.org", "Test1", $data);
    }
}

class TestHeader2 extends SoapHeader {
    function __construct($data) {
        parent::__construct("http://testuri.org", "Test2", $data);
    }
}

function test() {
    global $server;
    $server->addSoapHeader(new TestHeader1("Hello Header!"));
    $server->addSoapHeader(new TestHeader2("Hello Header!"));
    return "Hello Body!";
}

$server = new soapserver(__DIR__."/server025.wsdl");
$server->addfunction("test");

$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0" encoding="ISO-8859-1"?>
<SOAP-ENV:Envelope
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">
  <SOAP-ENV:Body>
    <ns1:test xmlns:ns1="http://testuri.org"/>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
EOF;

$server->handle($HTTP_RAW_POST_DATA);
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns1="http://testuri.org"><SOAP-ENV:Header><ns1:Test1 xsi:type="xsd:string">Hello Header!</ns1:Test1><ns1:Test2 xsi:type="xsd:string">Hello Header!</ns1:Test2></SOAP-ENV:Header><SOAP-ENV:Body><ns1:testResponse><result>Hello Body!</result></ns1:testResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
