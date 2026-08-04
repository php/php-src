--TEST--
GH-22285 (SoapServer dispatches to the first function when the SOAPAction header is empty)
--CREDITS--
Jarkko Hyvärinen
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--SKIPIF--
<?php
    if (php_sapi_name()=='cli') echo 'skip';
?>
--POST--
<SOAP-ENV:Envelope
    xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
    xmlns:ns1="urn:TestService">
    <SOAP-ENV:Body>
        <ns1:goodbyeIn>
            <name>World</name>
        </ns1:goodbyeIn>
    </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
--FILE--
<?php
class TestWS {
    public function hello($params) {
        return ['message' => 'Hello ' . $params->name];
    }
    public function goodbye($params) {
        return ['message' => 'Goodbye ' . $params->name];
    }
}

$server = new SoapServer(__DIR__ . '/gh22285.wsdl', [
    'cache_wsdl'   => WSDL_CACHE_NONE,
    'encoding'     => 'UTF-8',
    'soap_version' => SOAP_1_1,
]);
$server->setClass('TestWS');
$_SERVER['HTTP_SOAPACTION'] = '""';
$server->handle();
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:TestService"><SOAP-ENV:Body><ns1:goodbyeOut><message>Goodbye World</message></ns1:goodbyeOut></SOAP-ENV:Body></SOAP-ENV:Envelope>
