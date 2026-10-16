--TEST--
SoapClient reports "Nesting level too deep" as SoapFault without a bailout
--EXTENSIONS--
soap
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) {
    die('skip ASAN needs different stack limit setting due to more stack space usage');
}
?>
--FILE--
<?php
class TestSoapClient extends SoapClient {
    public string $response;

    public function __doRequest($request, $location, $action, $version, $oneWay = false, ?string $uriParserClass = null): string {
        return $this->response;
    }
}

$client = new TestSoapClient(null, ['location' => 'test://', 'uri' => 'urn:test']);

// An array that contains itself 1 and 8 times: decoding stops at the first error
foreach ([1, 8] as $n) {
    $client->response = '<?xml version="1.0"?>'
        . '<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"'
        . ' xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsd="http://www.w3.org/2001/XMLSchema"'
        . ' xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><SOAP-ENV:Body><ns1:testResponse xmlns:ns1="urn:test">'
        . '<return xsi:type="SOAP-ENC:Array" SOAP-ENC:arrayType="xsd:anyType[' . $n . ']" id="x">'
        . str_repeat('<i href="#x"/>', $n)
        . '</return></ns1:testResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>';
    try {
        $client->test();
    } catch (SoapFault $e) {
        echo "$n: $e->faultcode: $e->faultstring\n";
    }
}
var_dump(gc_status()['protected']);
?>
--EXPECT--
1: Client: SOAP-ERROR: Encoding: Nesting level too deep
8: Client: SOAP-ERROR: Encoding: Nesting level too deep
bool(false)
