--TEST--
Bug #76348 (WSDL_CACHE_MEMORY causes Segmentation fault)
--EXTENSIONS--
soap
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) die("skip online test");
?>
--FILE--
<?php
$client = new SoapClient(__DIR__ . DIRECTORY_SEPARATOR . 'bug76348.wsdl', [
    'cache_wsdl' => WSDL_CACHE_MEMORY,
]);
?>
===DONE===
--EXPECT--
===DONE===
