--TEST--
DOMNode::replaceChild() edge cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString("<!DOCTYPE html><html></html>");
$dom->documentElement->remove();
$parent = $dom->createElement("parent");
$child = $dom->createElement("child");
$parent->appendChild($child);
$dom->appendChild($parent);

echo "--- Wrong parent node type ---\n";

$comment = $dom->createComment('This is a comment');
try {
    $comment->replaceChild($comment, $dom->createElement("old-child"));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Node is an inclusive ancestor of parent ---\n";

try {
    $parent->replaceChild($parent, $child);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $parent->replaceChild($dom, $child);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Child's parent is not parent ---\n";

try {
    $parent->replaceChild($dom->createElement("new-child"), $dom->createElement("old-child"));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Invalid child to replace with ---\n";

try {
    $parent->replaceChild(new DOMEntityReference("foo"), $child);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Replace element with text in document root ---\n";

try {
    $dom->replaceChild(new DOMText("text"), $parent);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Replace child element with doctype inside element ---\n";

try {
    $parent->replaceChild($dom->doctype, $child);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Replace element with fragment containing multiple elements ---\n";

$fragment = $dom->createDocumentFragment();
$fragment->appendChild($dom->createElement("new-child1"));
$fragment->appendChild($dom->createElement("new-child2"));

try {
    $dom->replaceChild($fragment, $parent);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Replace comment in document causing more than two elements ---\n";

$comment = $dom->appendChild($dom->createComment("comment"));
try {
    $dom->replaceChild($dom->createElement("new-child"), $comment);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Replace dtd with element ---\n";

try {
    $dom->replaceChild($dom->createElement("new-child"), $dom->doctype);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Replace element with another dtd ---\n";

try {
    $dom->replaceChild($dom->doctype, $parent);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Replace parent with itself ---\n";

$dom->replaceChild($parent, $parent);
echo $dom->saveHTML(), "\n";

echo "--- Replace parent with single-child fragment ---\n";

$fragment = $dom->createDocumentFragment();
$fragment->appendChild($dom->createElement("new-child"));
$dom->replaceChild($fragment, $parent);
echo $dom->saveHTML(), "\n";

?>
--EXPECT--
--- Wrong parent node type ---
Hierarchy Request Error
--- Node is an inclusive ancestor of parent ---
Hierarchy Request Error
Hierarchy Request Error
--- Child's parent is not parent ---
Not Found Error
--- Invalid child to replace with ---
Hierarchy Request Error
--- Replace element with text in document root ---
Cannot insert text as a child of a document
--- Replace child element with doctype inside element ---
Cannot insert a document type into anything other than a document
--- Replace element with fragment containing multiple elements ---
Cannot have more than one element child in a document
--- Replace comment in document causing more than two elements ---
Cannot have more than one element child in a document
--- Replace dtd with element ---
Cannot have more than one element child in a document
--- Replace element with another dtd ---
Document types must be the first child in a document
--- Replace parent with itself ---
<!DOCTYPE html><parent><child></child></parent><!--comment-->
--- Replace parent with single-child fragment ---
<!DOCTYPE html><new-child></new-child><!--comment-->
