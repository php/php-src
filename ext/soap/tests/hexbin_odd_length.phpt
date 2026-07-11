--TEST--
SOAP rejects odd-length xsd:hexBinary values
--EXTENSIONS--
soap
--FILE--
<?php
class TestSoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $one_way = false, ?string $uriParserClass = null): string {
        return <<<'XML'
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
                   xmlns:xsd="http://www.w3.org/2001/XMLSchema"
                   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <SOAP-ENV:Body>
    <ns1:testResponse xmlns:ns1="urn:test">
      <return xsi:type="xsd:hexBinary">ABC</return>
    </ns1:testResponse>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;
    }
}

$client = new TestSoapClient(null, [
    'location' => 'test://',
    'uri' => 'urn:test',
    'exceptions' => true,
]);

try {
    var_dump(bin2hex($client->test()));
} catch (SoapFault $e) {
    echo $e->faultstring, "\n";
}
?>
--EXPECT--
SOAP-ERROR: Encoding: Violation of encoding rules
