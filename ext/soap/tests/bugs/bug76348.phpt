--TEST--
Bug #76348 (WSDL_CACHE_MEMORY causes Segmentation fault)
--SKIPIF--
<?php
if (!extension_loaded('soap')) die('skip soap extension not available');
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
