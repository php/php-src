--TEST--
Test 3: Exception Test
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$dom = new domdocument;
$dom->load(__DIR__."/book.xml");
$rootNode = $dom->documentElement;
print "--- Catch exception with try/catch\n";
try {
    $rootNode->appendChild($rootNode);
} catch (domexception $e) {
    ob_start();
    var_dump($e);
    $contents = ob_get_contents();
    ob_end_clean();
    echo preg_replace('/object\(DOMElement\).+\{.*?\}/s', 'DOMElement', $contents);
}
print "--- Don't catch exception with try/catch\n";
$rootNode->appendChild($rootNode);


?>
--EXPECTF--
--- Catch exception with try/catch
object(DOMException)#%d (%d) {
  ["message":protected]=>
  string(23) "Hierarchy Request Error"
  ["string":"Exception":private]=>
  string(0) ""
  ["file":protected]=>
  string(%d) "%sdom003.php"
  ["line":protected]=>
  int(8)
  ["trace":"Exception":private]=>
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
        DOMElement
      }
    }
  }
  ["previous":"Exception":private]=>
  NULL
  ["code"]=>
  int(3)
}
--- Don't catch exception with try/catch

Fatal error: Uncaught DOMException: Hierarchy Request Error in %sdom003.php:%d
Stack trace:
#0 %sdom003.php(%d): DOMNode->appendChild(Object(DOMElement))
#1 {main}
  thrown in %sdom003.php on line %d
