--TEST--
SoapClient stops decoding response headers after an exception
--EXTENSIONS--
soap
--INI--
zend.exception_ignore_args=0
--FILE--
<?php
class TestSoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $oneWay = false, ?string $uriParserClass = null): string {
        return <<<'XML'
<?xml version="1.0"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
                   xmlns:xsd="http://www.w3.org/2001/XMLSchema"
                   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <SOAP-ENV:Header>
    <h1 xsi:type="xsd:int">1</h1>
    <h2 xsi:type="xsd:string">a</h2>
    <h3 xsi:type="xsd:int">3</h3>
  </SOAP-ENV:Header>
  <SOAP-ENV:Body>
    <ns1:testResponse xmlns:ns1="urn:test">
      <return xsi:type="xsd:int">1</return>
    </ns1:testResponse>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;
    }
}

function string_from_xml($xml) {
    throw new Exception("string_from_xml failed");
}

$client = new TestSoapClient(null, [
    'location' => 'test://',
    'uri' => 'urn:test',
    'typemap' => [[
        'type_ns' => 'http://www.w3.org/2001/XMLSchema',
        'type_name' => 'string',
        'from_xml' => 'string_from_xml',
    ]],
]);

$headers = [];
try {
    $client->__soapCall('test', [], null, null, $headers);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
    // The trace holds the output headers as they were when the exception was thrown
    var_dump($e->getTrace()[1]['args'][4]);
}
var_dump($headers);
?>
--EXPECT--
string_from_xml failed
array(1) {
  ["h1"]=>
  int(1)
}
array(1) {
  ["h1"]=>
  int(1)
}
