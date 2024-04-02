--TEST--
DOM\HTMLDocument::registerNodeClass 02
--EXTENSIONS--
dom
--FILE--
<?php

class Custom extends DOM\Document {
    public function foo() {
    }
}

$dom = DOM\HTMLDocument::createEmpty();
try {
    $dom->registerNodeClass("DOM\\Document", "Custom");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$element = $dom->appendChild($dom->createElement("foo"));
unset($dom);

var_dump(get_class($element->ownerDocument));

// Should fail
$element->ownerDocument->foo();

?>
--EXPECTF--
DOM\Document::registerNodeClass(): Argument #1 ($baseClass) must not be an abstract class
string(16) "DOM\HTMLDocument"

Fatal error: Uncaught Error: Call to undefined method DOM\HTMLDocument::foo() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
