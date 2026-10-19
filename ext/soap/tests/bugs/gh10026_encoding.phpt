--TEST--
GH-10026 (Garbage collection stops after exception in SoapClient): encoding errors
--EXTENSIONS--
soap
--FILE--
<?php
class Node {
    public $self;
}

function make_cycles(int $n): void {
    for ($i = 0; $i < $n; $i++) {
        $node = new Node;
        $node->self = $node;
    }
}

class TestSoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $oneWay = false, ?string $uriParserClass = null): string {
        return <<<'XML'
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
                   xmlns:xsd="http://www.w3.org/2001/XMLSchema"
                   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <SOAP-ENV:Body>
    <ns1:testResponse xmlns:ns1="urn:test">
      <return xsi:type="xsd:int">not a number</return>
    </ns1:testResponse>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;
    }
}

$client = new TestSoapClient(null, ['location' => 'test://', 'uri' => 'urn:test']);

// Encoding the request fails
try {
    $client->test("\xff");
} catch (SoapFault $e) {
    echo $e->faultcode, ": ", $e->getMessage(), "\n";
}
var_dump(gc_status()['protected']);

// Decoding the response fails
try {
    $client->test("ok");
} catch (SoapFault $e) {
    echo $e->faultcode, ": ", $e->getMessage(), "\n";
}
var_dump(gc_status()['protected']);

make_cycles(20000);
gc_collect_cycles();
var_dump(gc_status()['collected']);
?>
--EXPECT--
Client: SOAP-ERROR: Encoding: string '\xff...' is not a valid utf-8 string
bool(false)
Client: SOAP-ERROR: Encoding: Invalid value for type 'int'
bool(false)
int(20000)
