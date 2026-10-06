--TEST--
GH-22887 (heap out-of-bounds read while decoding an array with an empty arraySize)
--EXTENSIONS--
soap
--FILE--
<?php
class TestSoapClient extends SoapClient {
    public string $response = '';
    public string $request = '';

    public function __doRequest(string $request, string $location, string $action, int $version, bool $oneWay = false): ?string {
        $this->request = $request;
        return $this->response;
    }
}

function soap_response(int $version, string $attributes): string {
    $envelope = $version === SOAP_1_2
        ? 'http://www.w3.org/2003/05/soap-envelope'
        : 'http://schemas.xmlsoap.org/soap/envelope/';
    $encoding = $version === SOAP_1_2
        ? 'http://www.w3.org/2003/05/soap-encoding'
        : 'http://schemas.xmlsoap.org/soap/encoding/';

    return <<<XML
    <?xml version="1.0" encoding="UTF-8"?>
    <SOAP-ENV:Envelope xmlns:SOAP-ENV="$envelope"
                       xmlns:SOAP-ENC="$encoding"
                       xmlns:xsd="http://www.w3.org/2001/XMLSchema"
                       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                       xmlns:ns1="http://example.org/">
      <SOAP-ENV:Body>
        <ns1:testResponse SOAP-ENV:encodingStyle="$encoding">
          <return $attributes>
            <item xsi:type="xsd:string">first</item>
            <item xsi:type="xsd:string">second</item>
          </return>
        </ns1:testResponse>
      </SOAP-ENV:Body>
    </SOAP-ENV:Envelope>
    XML;
}

/* Decoding: an arraySize holding no dimension at all must not be trusted as a
 * zero-dimensional array. */
foreach ([SOAP_1_1 => '1.1', SOAP_1_2 => '1.2'] as $version => $label) {
    foreach ([
        'SOAP-ENC:arraySize=""',
        'SOAP-ENC:arraySize="   "',
        'SOAP-ENC:arraySize="abc"',
        'SOAP-ENC:itemType="xsd:string" SOAP-ENC:arraySize=""',
        'SOAP-ENC:arraySize="2"',
    ] as $attributes) {
        $client = new TestSoapClient(null, [
            'location' => 'test://',
            'uri' => 'http://example.org/',
            'soap_version' => $version,
        ]);
        $client->response = soap_response($version, $attributes);

        echo "SOAP $label, $attributes:", PHP_EOL;
        var_dump($client->test());
    }
}

/* Encoding: same thing for an arraySize coming from a WSDL. */
$client = new TestSoapClient(__DIR__ . '/gh22887.wsdl', [
    'cache_wsdl' => WSDL_CACHE_NONE,
    'soap_version' => SOAP_1_2,
]);
$client->test(['first', 'second']);

$request = new DOMDocument();
$request->loadXML($client->request);
$value = $request->getElementsByTagName('value')->item(0);
echo 'arraySize: ', var_export($value->getAttribute('enc:arraySize'), true), PHP_EOL;
echo 'items: ', $value->getElementsByTagName('item')->length, PHP_EOL;
?>
--EXPECT--
SOAP 1.1, SOAP-ENC:arraySize="":
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
SOAP 1.1, SOAP-ENC:arraySize="   ":
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
SOAP 1.1, SOAP-ENC:arraySize="abc":
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
SOAP 1.1, SOAP-ENC:itemType="xsd:string" SOAP-ENC:arraySize="":
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
SOAP 1.1, SOAP-ENC:arraySize="2":
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
SOAP 1.2, SOAP-ENC:arraySize="":
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
SOAP 1.2, SOAP-ENC:arraySize="   ":
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
SOAP 1.2, SOAP-ENC:arraySize="abc":
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
SOAP 1.2, SOAP-ENC:itemType="xsd:string" SOAP-ENC:arraySize="":
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
SOAP 1.2, SOAP-ENC:arraySize="2":
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
arraySize: '2'
items: 2
