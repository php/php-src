--TEST--
Test DOM\Document::$body setter errors
--EXTENSIONS--
dom
--FILE--
<?php

function testNormalReplace($cb)
{
    $dom = DOM\HTMLDocument::createFromString('<p>foo</p>', LIBXML_NOERROR);
    var_dump($dom->body?->nodeName);
    try {
        $dom->body = $cb($dom);
    } catch (DOMException $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($dom->body?->nodeName);
}

echo "--- Set body to NULL ---\n";
testNormalReplace(fn ($dom) => NULL);

echo "--- Wrong element tag in right namespace ---\n";
testNormalReplace(fn ($dom) => $dom->createElementNS("http://www.w3.org/1999/xhtml", "foo"));

echo "--- Right element tag in wrong namespace ---\n";
testNormalReplace(fn ($dom) => $dom->createElementNS("urn:a", "body"));

echo "--- Right element tag in no namespace ---\n";
testNormalReplace(fn ($dom) => $dom->createElementNS("", "frameset"));

echo "--- Set body without document element ---\n";
$dom = DOM\XMLDocument::createEmpty();
try {
    $dom->body = $dom->createElementNS("http://www.w3.org/1999/xhtml", "body");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->body?->nodeName);

?>
--EXPECT--
--- Set body to NULL ---
string(4) "BODY"
The new body must either be a body or a frameset tag
string(4) "BODY"
--- Wrong element tag in right namespace ---
string(4) "BODY"
The new body must either be a body or a frameset tag
string(4) "BODY"
--- Right element tag in wrong namespace ---
string(4) "BODY"
The new body must either be a body or a frameset tag
string(4) "BODY"
--- Right element tag in no namespace ---
string(4) "BODY"
The new body must either be a body or a frameset tag
string(4) "BODY"
--- Set body without document element ---
A body can only be set if there is a document element
NULL
