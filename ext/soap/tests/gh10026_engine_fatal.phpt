--TEST--
GH-10026 (Engine fatal errors converted to SoapFault keep the GC protected)
--EXTENSIONS--
soap
--INI--
memory_limit=8M
--FILE--
<?php
class TestSoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $oneWay = false): ?string {
        return str_repeat("x", 64 * 1024 * 1024);
    }
}

var_dump(gc_status()["protected"]);
$client = new TestSoapClient(null, ["location" => "test://", "uri" => "urn:test"]);
try {
    $client->test();
} catch (SoapFault $e) {
    echo $e->getMessage(), "\n";
}
var_dump(gc_status()["protected"]);
?>
--EXPECTF--
bool(false)
Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes)
bool(true)
