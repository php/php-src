--TEST--
DOMNode::contains()
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<!DOCTYPE HTML>
<html xmlns:test="some:ns">
    <head>
        <title>my title</title>
    </head>
    <body>
        <main>
            <p>Hello, world!</p>
            <p>Second paragraph</p>
            <div><p>container</p></div>
            <!-- comment -->
        </main>
    </body>
</html>
XML);

$xpath = new DOMXPath($dom);
$head = $xpath->query("//head")[0];
$main = $xpath->query("//main")[0];
$div = $xpath->query("//div")[0];

echo "--- False edge cases ---\n";

var_dump($dom->documentElement->contains(null));

try {
    var_dump($dom->contains(new stdClass));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "--- True cases ---\n";
var_dump($dom->documentElement->contains($head));
var_dump($dom->documentElement->contains($main));
var_dump($dom->contains($dom));
var_dump($main->contains($main));
var_dump($div->contains($div));
var_dump($main->contains($div));
var_dump($main->contains($main->firstElementChild));
var_dump($main->contains($div->firstElementChild));
var_dump($div->contains($div->firstElementChild));
var_dump($main->contains($main->firstElementChild->firstChild));
var_dump($dom->contains($dom->doctype));
var_dump($dom->contains($dom->doctype));
var_dump($dom->contains($dom->documentElement->getAttributeNode('xmlns:test')));
var_dump($dom->contains($main->lastChild));

echo "--- False cases ---\n";
var_dump($main->firstElementChild->contains($main));
var_dump($main->contains($head));
var_dump($div->contains($main));
var_dump($main->contains($head->firstElementChild));
var_dump($div->contains($main->firstElementChild));
var_dump($div->contains($main->firstElementChild->nextElementSibling));
var_dump($div->contains($main->lastChild));

echo "--- False, create element case ---\n";

$newElement = $dom->createElement('x');
var_dump($dom->documentElement->contains($newElement));

echo "--- Removal case ---\n";

$main->remove();
var_dump($main->contains($main));
var_dump($dom->contains($main));

?>
--EXPECT--
--- False edge cases ---
bool(false)
DOMNode::contains(): Argument #1 ($other) must be of type DOMNode|DOMNameSpaceNode|null, stdClass given
--- True cases ---
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
--- False cases ---
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
--- False, create element case ---
bool(false)
--- Removal case ---
bool(true)
bool(false)
