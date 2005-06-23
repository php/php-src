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
  string(%d) "%sdom003.php"
  ["line:protected"]=>
  int(8)
  ["trace:private"]=>
  array(1) {
    [0]=>
    array(6) {
      ["file"]=>
      string(%d) "%sdom003.php"
      ["line"]=>
      int(8)
      ["function"]=>
      string(11) "appendChild"
      ["class"]=>
      string(7) "DOMNode"
      ["type"]=>
      string(2) "->"
      ["args"]=>
      array(1) {
        [0]=>
        object(DOMElement)#%d (0) {
        }
      }
    }
  }
  ["code"]=>
  int(3)
}
--- Don't catch exception with try/catch

Fatal error: Uncaught exception 'DOMException' with message 'Hierarchy Request Error' in %sdom003.php:%d
Stack trace:
#0 %sdom003.php(13): DOMNode->appendChild(Object(DOMElement))
#1 {main}
  thrown in %sdom003.php on line %d
