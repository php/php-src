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
  [u"message":protected]=>
  unicode(23) "Hierarchy Request Error"
  [u"string":u"Exception":private]=>
  unicode(0) ""
  [u"file":protected]=>
  unicode(%d) "%sdom003.php"
  [u"line":protected]=>
  int(8)
  [u"trace":u"Exception":private]=>
  array(1) {
    [0]=>
    array(6) {
      [u"file"]=>
      unicode(%d) "%sdom003.php"
      [u"line"]=>
      int(8)
      [u"function"]=>
      unicode(11) "appendChild"
      [u"class"]=>
      unicode(7) "DOMNode"
      [u"type"]=>
      unicode(2) "->"
      [u"args"]=>
      array(1) {
        [0]=>
        object(DOMElement)#%d (0) {
        }
      }
    }
  }
  [u"code"]=>
  int(3)
}
--- Don't catch exception with try/catch

Fatal error: Uncaught exception 'DOMException' with message 'Hierarchy Request Error' in %sdom003.php:%d
Stack trace:
#0 %sdom003.php(13): DOMNode->appendChild(Object(DOMElement))
#1 {main}
  thrown in %sdom003.php on line %d
