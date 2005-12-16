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
  ["message":protected]=>
  string(20) "Wrong Document Error"
  ["string":"Exception":private]=>
  string(0) ""
  ["file":protected]=>
  string(%d) "%sdom_set_attr_node.php"
  ["line":protected]=>
  int(%d)
  ["trace":"Exception":private]=>
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
--UEXPECTF--
object(DOMException)#%d (6) {
  [u"message":protected]=>
  unicode(20) "Wrong Document Error"
  [u"string":u"Exception":private]=>
  unicode(0) ""
  [u"file":protected]=>
  unicode(%d) "%sdom_set_attr_node.php"
  [u"line":protected]=>
  int(22)
  [u"trace":u"Exception":private]=>
  array(1) {
    [0]=>
    array(6) {
      [u"file"]=>
      unicode(%d) "%sdom_set_attr_node.php"
      [u"line"]=>
      int(22)
      [u"function"]=>
      unicode(16) "setAttributeNode"
      [u"class"]=>
      unicode(10) "DOMElement"
      [u"type"]=>
      unicode(2) "->"
      [u"args"]=>
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
