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
object(domexception)#3 (7) {
  [""]=>
  string(23) "Hierarchy Request Error"
  [""]=>
  string(0) ""
  [""]=>
  string(%d) "%s"
  [""]=>
  int(8)
  [""]=>
  NULL
  ["code"]=>
  int(3)
  ["trace"]=>
  array(0) {
  }
}
--- Don't catch exception with try/catch

Fatal error: Uncaught exception 'domexception' with message 'Hierarchy Request Error' in %s/dom003.php:%d
Stack trace:
#0 {main}
  thrown in %s/dom003.php on line %d
