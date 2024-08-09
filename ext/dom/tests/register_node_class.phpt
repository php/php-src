--TEST--
Test: registerNodeClass()
--EXTENSIONS--
dom
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
try {
    print $attr->testit()."\n";
} catch (Error $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
myElement
HELLO Element
myAttribute
HELLO Attribute
DOMAttr
Call to undefined method DOMAttr::testit()
