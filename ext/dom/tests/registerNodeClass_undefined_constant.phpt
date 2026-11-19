--TEST--
registerNodeClass() with a class whose constant expressions cannot be evaluated
--EXTENSIONS--
dom
--FILE--
<?php

const STRING_CONSTANT = "abc";

class MyElement extends DOMElement {
    public $x = UNDEFINED_CONSTANT;
}

class MyTypedElement extends DOMElement {
    public int $x = STRING_CONSTANT;
}

class MyText extends DOMText {
    const C = UNDEFINED_CONSTANT;
}

class MyDocument extends DOMDocument {
    public static $x = UNDEFINED_CONSTANT;
}

class MyModernElement extends Dom\Element {
    public $x = UNDEFINED_CONSTANT;
}

function register($doc, string $base, string $class) {
    try {
        $doc->registerNodeClass($base, $class);
        echo "$class: registered\n";
    } catch (Error $e) {
        echo "$class: ", $e::class, ": ", $e->getMessage(), "\n";
    }
}

$doc = new DOMDocument;
$doc->loadXML('<root>text<a/></root>');
register($doc, 'DOMElement', 'MyElement');
register($doc, 'DOMElement', 'MyTypedElement');
register($doc, 'DOMText', 'MyText');
register($doc, 'DOMDocument', 'MyDocument');
$root = $doc->documentElement;
unset($doc);
var_dump(get_class($root));
var_dump(get_class($root->firstChild));
var_dump(get_class($root->ownerDocument));

$modern = Dom\XMLDocument::createFromString('<root><a/></root>');
register($modern, 'Dom\Element', 'MyModernElement');
var_dump(get_class($modern->documentElement));

define('UNDEFINED_CONSTANT', 1);
register($root->ownerDocument, 'DOMElement', 'MyElement');
register($modern, 'Dom\Element', 'MyModernElement');
var_dump(get_class($root->lastChild));
var_dump(get_class($modern->documentElement->firstChild));

?>
--EXPECT--
MyElement: Error: Undefined constant "UNDEFINED_CONSTANT"
MyTypedElement: TypeError: Cannot assign string to property MyTypedElement::$x of type int
MyText: Error: Undefined constant "UNDEFINED_CONSTANT"
MyDocument: Error: Undefined constant "UNDEFINED_CONSTANT"
string(10) "DOMElement"
string(7) "DOMText"
string(11) "DOMDocument"
MyModernElement: Error: Undefined constant "UNDEFINED_CONSTANT"
string(11) "Dom\Element"
MyElement: registered
MyModernElement: registered
string(9) "MyElement"
string(15) "MyModernElement"
