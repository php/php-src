--TEST--
Test writing Element::$innerHTML on XML documents - error cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString(<<<XML
<!DOCTYPE root [
    <!ENTITY foo "content">
]>
<root/>
XML);
$child = $dom->documentElement->appendChild($dom->createElementNS('urn:a', 'child'));
$original = $dom->saveXML();

function test($child, $html) {
    global $dom, $original;
    try {
        $child->innerHTML = $html;
    } catch (DOMException $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($dom->saveXML() === $original);
}

test($child, '&foo;');
test($child, '</root>');
test($child, '</root><foo/><!--');
test($child, '--></root><!--');
test($child, '<');
test($child, '<!ENTITY foo "content">');

?>
--EXPECT--
XML fragment is not well-formed
bool(true)
XML fragment is not well-formed
bool(true)
XML fragment is not well-formed
bool(true)
XML fragment is not well-formed
bool(true)
XML fragment is not well-formed
bool(true)
XML fragment is not well-formed
bool(true)
