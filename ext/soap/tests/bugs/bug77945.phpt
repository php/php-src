--TEST--
Bug #77945: Segmentation fault when constructing SoapClient with WSDL_CACHE_BOTH
--SKIPIF--
<?php
if (!extension_loaded('soap')) die('skip soap extension not available');
?>
--FILE--
<?php

// The important part is to have a restriction enumeration with value="".
$client = new SoapClient(__DIR__ . '/bug29236.wsdl', [
    'cache_wsdl' => WSDL_CACHE_BOTH
]);

?>
===DONE===
--EXPECT--
===DONE===
