--TEST--
GH-10026 (Garbage collection stops after exception in SoapClient)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
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

$wsdl = __DIR__ . '/gh10026.wsdl';
file_put_contents($wsdl, '<?xml version="1.0"?><definitions xmlns="http://schemas.xmlsoap.org/wsdl/"><message/></definitions>');

try {
    new SoapClient($wsdl);
} catch (SoapFault $e) {
    echo $e->faultcode, ": ", $e->getMessage(), "\n";
}

var_dump(gc_status()['protected']);
make_cycles(20000);
gc_collect_cycles();
var_dump(gc_status()['collected']);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh10026.wsdl');
?>
--EXPECT--
WSDL: SOAP-ERROR: Parsing WSDL: <message> has no name attribute
bool(false)
int(20000)
