--TEST--
SoapClient constructor option errors do not disable the cycle collector (GH-10026)
--EXTENSIONS--
soap
--FILE--
<?php
class MyClient extends SoapClient {}

$cases = [
    'no uri' => fn() => new SoapClient(null, ['location' => 'http://127.0.0.1:9/']),
    'no uri, exceptions off' => fn() => new SoapClient(null, ['location' => 'http://127.0.0.1:9/', 'exceptions' => false]),
    'no location' => fn() => new SoapClient(null, ['uri' => 'urn:x']),
    'no options' => fn() => new SoapClient(null),
    'invalid encoding' => fn() => new SoapClient(null, ['location' => 'http://127.0.0.1:9/', 'uri' => 'urn:x', 'encoding' => 'no-such-encoding']),
    'subclass' => fn() => new MyClient(null, ['uri' => 'urn:x']),
];

$fault = new ReflectionProperty(SoapClient::class, '__soap_fault');
foreach ($cases as $name => $case) {
    try {
        $case();
        echo "$name: no exception\n";
    } catch (SoapFault $e) {
        echo "$name: [{$e->faultcode}] {$e->getMessage()}\n";
    }
    var_dump(gc_status()['protected']);
}

/* The fault is also stored in __soap_fault, as before */
$client = (new ReflectionClass(SoapClient::class))->newInstanceWithoutConstructor();
try {
    $client->__construct(null, ['uri' => 'urn:x']);
} catch (SoapFault $e) {
    var_dump($fault->getValue($client) === $e);
}

for ($i = 0; $i < 20000; $i++) {
    $node = new stdClass();
    $node->self = $node;
}
unset($node);
gc_collect_cycles();
var_dump(gc_status()['collected'] >= 20000);
?>
--EXPECT--
no uri: [Client] SoapClient::__construct(): 'uri' option is required in nonWSDL mode
bool(false)
no uri, exceptions off: [Client] SoapClient::__construct(): 'uri' option is required in nonWSDL mode
bool(false)
no location: [Client] SoapClient::__construct(): 'location' option is required in nonWSDL mode
bool(false)
no options: [Client] SoapClient::__construct(): 'location' and 'uri' options are required in nonWSDL mode
bool(false)
invalid encoding: [Client] SoapClient::__construct(): Invalid 'encoding' option - 'no-such-encoding'
bool(false)
subclass: [Client] SoapClient::__construct(): 'location' option is required in nonWSDL mode
bool(false)
bool(true)
bool(true)
