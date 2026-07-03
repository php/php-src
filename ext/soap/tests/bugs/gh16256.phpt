--TEST--
GH-16256 (Assertion failure in ext/soap/php_encoding.c:460)
--EXTENSIONS--
soap
--FILE--
<?php
$classmap = [
    "bogus",
];
$wsdl = __DIR__."/ext/soap/tests/bug41004.wsdl";
try {
    new SoapClient($wsdl, ["classmap" => $classmap]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    new SoapServer($wsdl, ["classmap" => $classmap]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
SoapClient::__construct(): 'classmap' option must be an associative array
SoapServer::__construct(): Argument #2 ($options) "classmap" option must be an associative array
