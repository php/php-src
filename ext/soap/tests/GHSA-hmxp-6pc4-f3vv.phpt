--TEST--
GHSA-hmxp-6pc4-f3vv: Null pointer dereference on missing Apache map value
--CREDITS--
Ilia Alshanetsky (iliaal)
--EXTENSIONS--
soap
--FILE--
<?php

$request = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<soap:Envelope
    xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xsd="http://www.w3.org/2001/XMLSchema"
    xmlns:apache="http://xml.apache.org/xml-soap">

    <soap:Body>
        <test>
            <map xsi:type="apache:Map">
                <item><key>hello</key></item>
            </map>
        </test>
    </soap:Body>
</soap:Envelope>
XML;

$server = new SoapServer(null, [
    'uri' => 'urn:test',
    'typemap' => [['type_name' => 'anything']],
]);
$server->addFunction('test');
function test($m) { return null; }
$server->handle($request);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>SOAP-ERROR: Encoding: Can't decode apache map, missing value</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
