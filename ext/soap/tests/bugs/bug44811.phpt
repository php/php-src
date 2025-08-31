--TEST--
Bug #44811 (Improve error messages when creating new SoapClient which contains invalid data)
--EXTENSIONS--
soap
--SKIPIF--
<?php if (getenv("SKIP_ONLINE_TESTS")) die("skip online test"); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
try {
    $x = new SoapClient('https://php.net');
} catch (SoapFault $e) {
    echo $e->getMessage() . PHP_EOL;
}
die('ok');
?>
--EXPECTF--
SOAP-ERROR: Parsing WSDL: Couldn't load from 'https://php.net' : %s

ok
