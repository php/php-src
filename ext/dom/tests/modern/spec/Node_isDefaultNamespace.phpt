--TEST--
DOM\Node::isDefaultNamespace()
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE html>
<html>
    <body>
        <svg xmlns="http://www.w3.org/2000/svg" height="1"></svg>
        <math></math>
    </body>
</html>
HTML);

function dump($node) {
    echo "svg NS: ";
    var_dump($node->isDefaultNamespace('http://www.w3.org/2000/svg'));
    echo "xhtml NS: ";
    var_dump($node->isDefaultNamespace('http://www.w3.org/1999/xhtml'));
    echo "mathml NS: ";
    var_dump($node->isDefaultNamespace('http://www.w3.org/1998/Math/MathML'));
    echo "empty NS: ";
    var_dump($node->isDefaultNamespace(''));
}

echo "--- Document ---\n";
dump($dom);
echo "--- svg element ---\n";
dump($dom->getElementsByTagName('svg')[0]);
echo "--- math element ---\n";
dump($dom->getElementsByTagName('math')[0]);
echo "--- svg attribute ---\n";
dump($dom->getElementsByTagName('svg')[0]->attributes[0]);
echo "--- empty fragment ---\n";
dump($dom->createDocumentFragment());
echo "--- doctype ---\n";
dump($dom->doctype);
echo "--- detached attribute ---\n";
dump($dom->createAttribute('foo'));
echo "--- custom namespace with a prefix ---\n";
var_dump($dom->createElementNS('urn:a', 'q:name')->isDefaultNamespace('urn:a'));
echo "--- custom namespace with a prefix and with attributes ---\n";
$element = $dom->createElementNS('urn:a', 'q:name');
$element->setAttributeNS("urn:x", "x:foo", "");
$element->setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:q", "urn:test");
$element->setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns", "urn:test2");
var_dump($element->isDefaultNamespace('urn:a'));
var_dump($element->isDefaultNamespace('urn:test'));
var_dump($element->isDefaultNamespace('urn:test2'));
echo "--- custom namespace without a prefix ---\n";
var_dump($dom->createElementNS('urn:a', 'name')->isDefaultNamespace('urn:a'));

?>
--EXPECT--
--- Document ---
svg NS: bool(false)
xhtml NS: bool(true)
mathml NS: bool(false)
empty NS: bool(false)
--- svg element ---
svg NS: bool(true)
xhtml NS: bool(false)
mathml NS: bool(false)
empty NS: bool(false)
--- math element ---
svg NS: bool(false)
xhtml NS: bool(false)
mathml NS: bool(true)
empty NS: bool(false)
--- svg attribute ---
svg NS: bool(true)
xhtml NS: bool(false)
mathml NS: bool(false)
empty NS: bool(false)
--- empty fragment ---
svg NS: bool(false)
xhtml NS: bool(false)
mathml NS: bool(false)
empty NS: bool(true)
--- doctype ---
svg NS: bool(false)
xhtml NS: bool(false)
mathml NS: bool(false)
empty NS: bool(true)
--- detached attribute ---
svg NS: bool(false)
xhtml NS: bool(false)
mathml NS: bool(false)
empty NS: bool(true)
--- custom namespace with a prefix ---
bool(false)
--- custom namespace with a prefix and with attributes ---
bool(false)
bool(false)
bool(true)
--- custom namespace without a prefix ---
bool(true)
