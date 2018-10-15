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
echo get_class($root), "\n";
print $root->testit()."\n";
$attr = $root->getAttributeNode('a');
echo get_class($attr), "\n";
print $attr->testit()."\n";
unset($attr);
$doc->registerNodeClass('DOMAttr', NULL);
$attr = $root->getAttributeNode('a');
echo get_class($attr), "\n";
print $attr->testit()."\n";
?>
--EXPECTF--
myElement
HELLO Element
myAttribute
HELLO Attribute
DOMAttr

Fatal error: Uncaught Error: Call to undefined method DOMAttr::testit() in %s:25
Stack trace:
#0 {main}
  thrown in %s on line 25
