--TEST--
Test: setAttributeNode()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$xml = <<<HERE
<?xml version="1.0" ?>
<root a="b" />
HERE;

$xml2 = <<<HERE
<?xml version="1.0" ?>
<doc2 />
HERE;

$dom = new DOMDocument();
$dom->loadXML($xml);
$root = $dom->documentElement;
$attr = $root->getAttributeNode('a');

$dom2 = new DOMDocument();
$dom2->loadXML($xml2);
$root2 = $dom2->documentElement;
try {
   $root2->setAttributeNode($attr);
} catch (domexception $e) {
    var_dump($e);
} 

?>
--EXPECTF--
object(DOMException)#%d (6) {
  ["message:protected"]=>
  string(20) "Wrong Document Error"
  ["string:private"]=>
  string(0) ""
  ["file:protected"]=>
  string(%d) "%sdom_set_attr_node.php"
  ["line:protected"]=>
  int(%d)
  ["trace:private"]=>
  array(1) {
    [0]=>
    array(6) {
      ["file"]=>
      string(%d) "%sdom_set_attr_node.php"
      ["line"]=>
      int(%d)
      ["function"]=>
      string(16) "setAttributeNode"
      ["class"]=>
      string(10) "DOMElement"
      ["type"]=>
      string(2) "->"
      ["args"]=>
      array(1) {
        [0]=>
        object(DOMAttr)#%d (0) {
        }
      }
    }
  }
  ["code"]=>
  int(4)
}
