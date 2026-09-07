--TEST--
SoapClient must not read packed private classmap as string-keyed map
--EXTENSIONS--
soap
--FILE--
<?php

class LocalSoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $one_way = false, ?string $uriParserClass = null): string {
        echo "__doRequest called\n";

        return <<<'XML'
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">
  <SOAP-ENV:Body>
    <SOAP-ENV:Fault>
      <faultcode>SOAP-ENV:Server</faultcode>
      <faultstring>expected fault</faultstring>
    </SOAP-ENV:Fault>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;
    }
}

class Foo {}

$client = new LocalSoapClient(null, [
    'location' => 'http://example.org/',
    'uri' => 'http://example.org/',
]);

$property = new ReflectionProperty(SoapClient::class, '_classmap');
$property->setValue($client, ['Foo']);

try {
    $client->__soapCall('foo', [new Foo()]);
} catch (SoapFault) {
    echo "SOAP Fault thrown\n";
}

?>
--EXPECT--
__doRequest called
SOAP Fault thrown
