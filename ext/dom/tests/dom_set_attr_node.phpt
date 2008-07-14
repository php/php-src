--TEST--
Test: setAttributeNode()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$xml = b<<<HERE
<?xml version="1.0" ?>
<root a="b" />
HERE;

$xml2 = b<<<HERE
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
object(DOMException)#%d (%d) {
  [u"message":protected]=>
  unicode(20) "Wrong Document Error"
  [u"string":u"Exception":private]=>
  unicode(0) ""
  [u"file":protected]=>
  unicode(%d) "%sdom_set_attr_node.php"
  [u"line":protected]=>
  int(%d)
  [u"trace":u"Exception":private]=>
  array(1) {
    [0]=>
    array(6) {
      [u"file"]=>
      unicode(%d) "%sdom_set_attr_node.php"
      [u"line"]=>
      int(%d)
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
  [u"previous":u"Exception":private]=>
  NULL
  [u"code"]=>
  int(4)
}
