--TEST--
DOM\Node::lookupNamespaceURI()
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

$body = $dom->getElementsByTagName("body")[0];
$namespaceless = $body->appendChild($dom->createElementNS(NULL, "foo"));
$prefixed = $body->appendChild($dom->createElementNS("urn:a", "a:a"));

echo "--- Hardcoded prefixes ---\n";
var_dump($dom->lookupNamespaceURI("xml"));
var_dump($dom->lookupNamespaceURI("xmlns"));

echo "--- Default prefix ---\n";
var_dump($dom->lookupNamespaceURI(""));
var_dump($dom->lookupNamespaceURI(NULL));

echo "--- NULL namespace should propagate up ---\n";
var_dump($namespaceless->lookupNamespaceURI(""));
var_dump($namespaceless->lookupNamespaceURI(NULL));
var_dump($namespaceless->lookupNamespaceURI("a"));

echo "--- Prefixed element ---\n";
var_dump($prefixed->lookupNamespaceURI(""));
var_dump($prefixed->lookupNamespaceURI(NULL));
var_dump($prefixed->lookupNamespaceURI("a"));

echo "--- Prefixed element custom xmlns attribute should not change ns ---\n";
$prefixed->setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:a", "urn:another");
var_dump($prefixed->lookupNamespaceURI(""));
var_dump($prefixed->lookupNamespaceURI(NULL));
var_dump($prefixed->lookupNamespaceURI("a"));

echo "--- xmlns attribute defines new namespace ---\n";
$body->setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:a", "urn:another");
var_dump($body->lookupNamespaceURI(""));
var_dump($body->lookupNamespaceURI(NULL));
var_dump($body->lookupNamespaceURI("a"));

echo "--- empty xmlns attribute defines no new namespace ---\n";
$body->setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:a", "");
var_dump($body->lookupNamespaceURI(""));
var_dump($body->lookupNamespaceURI(NULL));
var_dump($body->lookupNamespaceURI("a"));

echo "--- lookup in empty document ---\n";
$dom = DOM\HTMLDocument::createEmpty();
var_dump($dom->lookupNamespaceURI(""));
var_dump($dom->lookupNamespaceURI(NULL));
var_dump($dom->lookupNamespaceURI("a"));

?>
--EXPECT--
--- Hardcoded prefixes ---
string(36) "http://www.w3.org/XML/1998/namespace"
string(29) "http://www.w3.org/2000/xmlns/"
--- Default prefix ---
string(28) "http://www.w3.org/1999/xhtml"
string(28) "http://www.w3.org/1999/xhtml"
--- NULL namespace should propagate up ---
string(28) "http://www.w3.org/1999/xhtml"
string(28) "http://www.w3.org/1999/xhtml"
NULL
--- Prefixed element ---
string(28) "http://www.w3.org/1999/xhtml"
string(28) "http://www.w3.org/1999/xhtml"
string(5) "urn:a"
--- Prefixed element custom xmlns attribute should not change ns ---
string(28) "http://www.w3.org/1999/xhtml"
string(28) "http://www.w3.org/1999/xhtml"
string(5) "urn:a"
--- xmlns attribute defines new namespace ---
string(28) "http://www.w3.org/1999/xhtml"
string(28) "http://www.w3.org/1999/xhtml"
string(11) "urn:another"
--- empty xmlns attribute defines no new namespace ---
string(28) "http://www.w3.org/1999/xhtml"
string(28) "http://www.w3.org/1999/xhtml"
NULL
--- lookup in empty document ---
NULL
NULL
NULL
