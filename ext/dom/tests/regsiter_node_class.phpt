--TEST--
Test: registerNodeClass()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class myAttribute extends DOMAttr {
   function testit() { return "HELLO Attribute"; }
}

class myElement extends DOMElement {
   function testit() { return "HELLO Element"; }
}

$doc = new DOMDocument();
$doc->registerNodeClass('DOMAttr', 'myAttribute');
$doc->registerNodeClass('DOMElement', 'myElement');
$doc->appendChild(new DOMElement('root'));
$root = $doc->documentElement;
$root->setAttribute('a', 'a1');
var_dump($root);
print $root->testit()."\n";
$attr = $root->getAttributeNode('a');
var_dump($attr);
print $attr->testit()."\n";
unset($attr);
$doc->registerNodeClass('DOMAttr', NULL);
$attr = $root->getAttributeNode('a');
var_dump($attr);
print $attr->testit()."\n";
?>
--EXPECTF--

object(myElement)#%d (0) {
}
HELLO Element
object(myAttribute)#%d (0) {
}
HELLO Attribute
object(DOMAttr)#%d (0) {
}

Fatal error: Call to undefined method DOMAttr::testit() in %s on line 25
