--TEST--
SOAP array index overflow is rejected
--EXTENSIONS--
soap
--FILE--
<?php
class TestSoapClient extends SoapClient {
    public string $response;

    public function __doRequest($request, $location, $action, $version, $one_way = false, ?string $uriParserClass = null): string {
        return $this->response;
    }
}

function soap_response(string $attributes, string $itemAttributes = ''): string {
    return <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
                   xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
                   xmlns:xsd="http://www.w3.org/2001/XMLSchema"
                   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                   xmlns:ns1="http://example.org/"
                   SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
  <SOAP-ENV:Body>
    <ns1:testResponse>
      <return $attributes>
        <item xsi:type="xsd:string" $itemAttributes>value</item>
      </return>
    </ns1:testResponse>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;
}

function test_overflow(string $name, string $response): void {
    $client = new TestSoapClient(NULL, [
        'location' => 'test://',
        'uri' => 'http://example.org/',
        'exceptions' => true,
    ]);
    $client->response = $response;

    try {
        $client->test();
        echo "$name: no fault\n";
    } catch (SoapFault $e) {
        echo "$name: $e->faultstring\n";
    }
}

function test_boundary_position(): void {
    $client = new TestSoapClient(NULL, [
        'location' => 'test://',
        'uri' => 'http://example.org/',
        'exceptions' => true,
    ]);
    $client->response = soap_response(
        'SOAP-ENC:arrayType="xsd:string[1]" xsi:type="SOAP-ENC:Array"',
        'SOAP-ENC:position="[2147483646]"'
    );

    var_dump($client->test());
}

test_overflow(
    'arrayType',
    soap_response('SOAP-ENC:arrayType="xsd:string[2147483648]" xsi:type="SOAP-ENC:Array"')
);

test_overflow(
    'offset',
    soap_response('SOAP-ENC:arrayType="xsd:string[1]" SOAP-ENC:offset="[2147483648]" xsi:type="SOAP-ENC:Array"')
);

test_overflow(
    'position',
    soap_response('SOAP-ENC:arrayType="xsd:string[1]" xsi:type="SOAP-ENC:Array"', 'SOAP-ENC:position="[2147483647]"')
);

test_boundary_position();
?>
--EXPECT--
arrayType: SOAP-ERROR: Encoding: array index out of range
offset: SOAP-ERROR: Encoding: array index out of range
position: SOAP-ERROR: Encoding: array index out of range
array(1) {
  [2147483646]=>
  string(5) "value"
}
