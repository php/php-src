--TEST--
GHSA-m33r-qmcv-p97q: Use-after-free after header parsing failure with SOAP_PERSISTENCE_SESSION
--CREDITS--
Ilia Alshanetsky (iliaal)
--EXTENSIONS--
soap
session
--FILE--
<?php

class Handler {
    public function return()  {
        return new SoapFault('Server', 'denied');
    }
    public function throw()  {
        throw new SoapFault('Server', 'denied');
    }
    public function hello() {
        return 'ok';
    }
}

session_start();

$srv = new SoapServer(null, ['uri' => 'urn:a']);
$srv->setClass(Handler::class);
$srv->setPersistence(SOAP_PERSISTENCE_SESSION);

$srv->handle(<<<XML
<?xml version="1.0" encoding="UTF-8"?>
<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/" xmlns:a="urn:a">
    <soap:Header>
        <a:return/>
    </soap:Header>
    <soap:Body>
        <a:hello/>
    </soap:Body>
</soap:Envelope>
XML);

$srv->handle(<<<XML
<?xml version="1.0" encoding="UTF-8"?>
<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/" xmlns:a="urn:a">
    <soap:Header>
        <a:throw/>
    </soap:Header>
    <soap:Body>
        <a:hello/>
    </soap:Body>
</soap:Envelope>
XML);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>denied</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>denied</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
