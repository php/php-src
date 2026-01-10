--TEST--
Unsetting properties
--EXTENSIONS--
dom
--FILE--
<?php

class MyElement extends DOMElement {
    public int $myProp = 3;
}

$dom = new DOMDocument;
$dom->registerNodeClass('DOMElement', 'MyElement');
$dom->loadXML('<root>foo</root>');
$root = $dom->documentElement;

unset($root->myProp);
try {
    $root->myProp;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($root->textContent);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Typed property MyElement::$myProp must not be accessed before initialization
Cannot unset MyElement::$textContent
