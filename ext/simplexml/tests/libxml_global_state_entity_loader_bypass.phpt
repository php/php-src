--TEST--
GHSA-3qrf-m4j2-pcrr (libxml global state entity loader bypass)
--SKIPIF--
<?php
if (!extension_loaded('libxml')) die('skip libxml extension not available');
if (!extension_loaded('simplexml')) die('skip simplexml extension not available');
if (!extension_loaded('zend-test')) die('skip zend-test extension not available');
if (!function_exists('zend_test_override_libxml_global_state')) die('skip not for Windows');
?>
--FILE--
<?php

$xml = "<?xml version='1.0'?><!DOCTYPE root [<!ENTITY % bork SYSTEM \"php://nope\"> %bork;]><nothing/>";

libxml_use_internal_errors(true);
zend_test_override_libxml_global_state();

echo "--- String test ---\n";
simplexml_load_string($xml);
echo "--- Constructor test ---\n";
new SimpleXMLElement($xml);
echo "--- File test ---\n";
file_put_contents("libxml_global_state_entity_loader_bypass.tmp", $xml);
simplexml_load_file("libxml_global_state_entity_loader_bypass.tmp");

echo "Done\n";

?>
--CLEAN--
<?php
@unlink("libxml_global_state_entity_loader_bypass.tmp");
?>
--EXPECT--
--- String test ---
--- Constructor test ---
--- File test ---
Done
