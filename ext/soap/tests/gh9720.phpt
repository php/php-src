--TEST--
Bug GH-9720 (Null pointer dereference while serializing the response)
--EXTENSIONS--
soap
--FILE--
<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);
ini_set("soap.wsdl_cache_enabled", 0);

class SoapService {
    function openSession($user) {
        return ["OK", "200"];
    }
}

$server = new SoapServer(__DIR__ . '/gh9720.wsdl');
$server->setClass(SoapService::class);
$request = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:soapService" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/">
    <SOAP-ENV:Body>
        <ns1:openSession>
            <user xsi:type="xsd:string">istoph</user>
        </ns1:openSession>
    </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;

$server->handle($request);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:soapService" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:openSessionResponse><status xsi:type="xsd:string">OK</status><error_code xsi:type="xsd:string">200</error_code></ns1:openSessionResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
