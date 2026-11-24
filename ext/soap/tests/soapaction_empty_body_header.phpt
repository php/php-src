--TEST--
SoapServer::handle() with Header, empty Body and SOAPAction matching an RPC operation
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--POST--
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org">
    <SOAP-ENV:Header>
        <ns1:Auth SOAP-ENV:mustUnderstand="1">secret</ns1:Auth>
    </SOAP-ENV:Header>
    <SOAP-ENV:Body/>
</SOAP-ENV:Envelope>
--FILE--
<?php
class Service {
    private string $auth = 'none';

    public function Auth($value) {
        $this->auth = $value;
    }

    public function test() {
        return "auth: " . $this->auth;
    }
}

$server = new SoapServer(__DIR__.'/soapaction_empty_body_header.wsdl');
$server->setObject(new Service());
$_SERVER["HTTP_SOAPACTION"] = "Test";
$server->handle();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org"><SOAP-ENV:Body><ns1:testResponse><result>auth: secret</result></ns1:testResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
