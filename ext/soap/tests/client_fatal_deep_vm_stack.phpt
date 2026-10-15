--TEST--
SoapClient: fatal error caught after the VM stack grew restores EG(vm_stack_top)
--EXTENSIONS--
soap
--FILE--
<?php
function recurse($depth) {
    if ($depth > 0) {
        return recurse($depth - 1);
    }
    eval("class Dup {} class Dup {}");
}

class TestClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $oneWay = false, $uriParserClass = null): ?string {
        recurse(50000);
        return "";
    }
}

$client = new TestClient(null, ["location" => "test://", "uri" => "test://"]);
try {
    $client->test();
} catch (SoapFault $e) {
    echo $e->getMessage(), "\n";
}

function f($a, $b, $c) {
    return [$a, $b, $c];
}
for ($i = 0; $i < 100000; $i++) {
    f($i, [$i], "x$i");
}
echo "Done\n";
?>
--EXPECTF--
Cannot redeclare class Dup (previously declared in %s)
Done
