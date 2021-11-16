--TEST--
Bug #74950 (null pointer deref in zim_simplexml_element_getDocNamespaces)
--EXTENSIONS--
simplexml
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only");
?>
--FILE--
<?php
$xml=new SimpleXMLElement(0,9000000000);var_dump($xml->getDocNamespaces())?>
?>
--EXPECTF--
Fatal error: Uncaught Exception: SimpleXMLElement::__construct(): Argument #2 ($options) is invalid in %sbug74950.php:%d
Stack trace:
#0 %sbug74950.php(%d): SimpleXMLElement->__construct('0', 9000000000)
#1 {main}
  thrown in %sbug74950.php on line %d
