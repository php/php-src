--TEST--
SOAP reports specific scalar encoding errors
--EXTENSIONS--
soap
--FILE--
<?php
class ScalarErrorClient extends SoapClient {
    public string $response;

    public function __doRequest($request, $location, $action, $version, $one_way = false, ?string $uriParserClass = null): string {
        return $this->response;
    }
}

function soap_response(string $type, string $value): string {
    return <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
                   xmlns:xsd="http://www.w3.org/2001/XMLSchema"
                   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <SOAP-ENV:Body>
    <ns1:testResponse xmlns:ns1="urn:test">
      <return xsi:type="$type">$value</return>
    </ns1:testResponse>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;
}

function run_case(string $label, string $type, string $value): void {
    $client = new ScalarErrorClient(null, [
        'location' => 'test://',
        'uri' => 'urn:test',
        'exceptions' => true,
    ]);
    $client->response = soap_response($type, $value);

    try {
        $client->__soapCall('test', []);
    } catch (SoapFault $e) {
        echo $label, ': ', $e->faultstring, "\n";
    }
}

run_case('double', 'xsd:double', 'abc');
run_case('long', 'xsd:long', 'abc');
?>
--EXPECT--
double: SOAP-ERROR: Encoding: Invalid value for type 'double'
long: SOAP-ERROR: Encoding: Invalid value for type 'long'
