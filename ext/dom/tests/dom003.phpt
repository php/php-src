--TEST--
Test 3: Exception Test
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$dom = new domdocument;
$dom->load(dirname(__FILE__)."/book.xml");
$rootNode = $dom->documentElement;
print "--- Catch exception with try/catch\n";
try {
    $rootNode->appendChild($rootNode);
} catch (domexception $e) {
    var_dump($e);
}
print "--- Don't catch exception with try/catch\n";
$rootNode->appendChild($rootNode);


?>
--EXPECTF--
--- Catch exception with try/catch
object(DOMException)#%d (6) {
  ["message:protected"]=>
  string(23) "Hierarchy Request Error"
  ["string:private"]=>
  string(0) ""
  ["file:protected"]=>
  string(%d) "%s/ext/dom/tests/dom003.php"
  ["line:protected"]=>
  int(8)
  ["trace:private"]=>
  array(0) {
  }
  ["code"]=>
  int(3)
}
--- Don't catch exception with try/catch

Fatal error: Uncaught exception 'DOMException' with message 'Hierarchy Request Error' in %sdom003.php:%d
Stack trace:
#0 {main}
  thrown in %sdom003.php on line %d
