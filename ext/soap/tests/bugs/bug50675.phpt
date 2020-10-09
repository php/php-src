--TEST--
Bug #50675 SoapClient can't handle object references correctly.
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

class TestSoapClient extends SoapClient {
  function __doRequest($request, $location, $action, $version, $one_way = 0) {
    return <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<soapenv:Envelope
xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/"
xmlns:xsd="http://www.w3.org/2001/XMLSchema"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
    <soapenv:Body>
        <soapenv:Fault>
            <faultcode>soapenv:Server.userException</faultcode>
            <faultstring>service.EchoServiceException</faultstring>
            <detail>
                <service.EchoServiceException xsi:type="ns1:EchoServiceException" xmlns:ns1="urn:service.EchoService">
                    <intParameter xsi:type="xsd:int">105</intParameter>
                    <parameter xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">string param</parameter>
                </service.EchoServiceException>
                <ns2:hostname xmlns:ns2="http://xml.apache.org/axis/">steckovic</ns2:hostname>
            </detail>
        </soapenv:Fault>
    </soapenv:Body>
</soapenv:Envelope>
EOF;
    }
}

ini_set('soap.wsdl_cache_enabled', 0);

$parameters = [
    'trace' => 1,
    'exceptions' => 0,
];
$client = new TestSoapClient(__DIR__ . '/bug50675.wsdl', $parameters);

$person = new stdClass();
$person->name = 'name';

$result = $client->echoPerson($person, $person);

print($client->__getLastRequest());
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://service" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="urn:service.EchoService" xmlns:xsd="http://www.w3.org/2001/XMLSchema" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoPerson><p xsi:type="ns2:Person" id="ref1"><name xsi:type="SOAP-ENC:string">name</name></p><p2 href="#ref1"/></ns1:echoPerson></SOAP-ENV:Body></SOAP-ENV:Envelope>
