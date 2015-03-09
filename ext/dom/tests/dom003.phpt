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
  ["string":"BaseException":private]=>
  string(0) ""
  ["file":protected]=>
  string(%d) "%sdom003.php"
  ["line":protected]=>
  int(8)
  ["trace":"BaseException":private]=>
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
  ["previous":"BaseException":private]=>
  NULL
  ["code"]=>
  int(3)
}
--- Don't catch exception with try/catch

Fatal error: Uncaught exception 'DOMException' with message 'Hierarchy Request Error' in %sdom003.php:%d
Stack trace:
#0 %sdom003.php(%d): DOMNode->appendChild(Object(DOMElement))
#1 {main}
  thrown in %sdom003.php on line %d
