--TEST--
GH-10026 (WSDL errors raised by ext/soap must not leave the GC protected)
--EXTENSIONS--
soap
--FILE--
<?php
$wsdl = __DIR__ . "/gh10026_protected.wsdl";
file_put_contents($wsdl, "not a wsdl at all");

var_dump(gc_status()["protected"]);
try {
    new SoapClient($wsdl, ["cache_wsdl" => WSDL_CACHE_NONE]);
} catch (SoapFault $e) {
    echo trim($e->getMessage()), "\n";
}
var_dump(gc_status()["protected"]);
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/gh10026_protected.wsdl");
?>
--EXPECTF--
bool(false)
SOAP-ERROR: Parsing WSDL: Couldn't load from %s
bool(false)
