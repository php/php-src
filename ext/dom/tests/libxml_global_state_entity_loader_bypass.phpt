--TEST--
GHSA-3qrf-m4j2-pcrr (libxml global state entity loader bypass)
--SKIPIF--
<?php
if (!extension_loaded('libxml')) die('skip libxml extension not available');
if (!extension_loaded('dom')) die('skip dom extension not available');
if (!extension_loaded('zend-test')) die('skip zend-test extension not available');
if (!function_exists('zend_test_override_libxml_global_state')) die('skip not for Windows');
?>
--FILE--
<?php

$xml = "<?xml version='1.0'?><!DOCTYPE root [<!ENTITY % bork SYSTEM \"php://nope\"> %bork;]><nothing/>";

libxml_use_internal_errors(true);

function parseXML($xml) {
  $doc = new DOMDocument();
  @$doc->loadXML($xml);
  $doc->createDocumentFragment()->appendXML("&bork;");
  foreach (libxml_get_errors() as $error) {
    var_dump(trim($error->message));
  }
}

parseXML($xml);
zend_test_override_libxml_global_state();
parseXML($xml);

echo "Done\n";

?>
--EXPECT--
string(25) "Entity 'bork' not defined"
string(25) "Entity 'bork' not defined"
string(25) "Entity 'bork' not defined"
Done
