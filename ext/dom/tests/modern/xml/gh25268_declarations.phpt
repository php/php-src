--TEST--
GH-25268 (NULL php_libxml_ref_obj dereference in dom_objects_store_clone_obj when cloning DTD declarations)
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\XMLDocument::createFromString('<!DOCTYPE root [<!ELEMENT root EMPTY><!ENTITY e "x">]><root/>');

foreach ([$dom->doctype->entities->getNamedItem("e"), $dom->doctype->childNodes[0]] as $decl) {
    $clone = clone $decl;
    try {
        var_dump($clone->nodeName);
    } catch (DOMException $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    try {
        var_dump($decl->cloneNode());
    } catch (DOMException $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}
?>
--EXPECT--
DOMException: Invalid State Error
DOMException: Invalid State Error
DOMException: Invalid State Error
DOMException: Invalid State Error
