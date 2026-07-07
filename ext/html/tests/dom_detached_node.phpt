--TEST--
Html: rendering a detached DOM node (no ownerDocument) fails cleanly, no bad free
--EXTENSIONS--
html
dom
--FILE--
<?php
// A standalone DOMDocumentType has no ownerDocument, so the internal
// ownerDocument read fails and leaves the read buffer untouched. Rendering
// must fail with a catchable Error (no serializer on the node itself),
// never a bad free / crash on the uninitialized buffer.
$imp = new DOMImplementation();
$dtd = $imp->createDocumentType('html');
var_dump($dtd->ownerDocument);

try {
    echo (string)(new \Html\Element("div", [], [$dtd]));
} catch (Error $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
NULL
Error: Cannot serialize DOMDocumentType to HTML: DOMDocumentType has no saveHtml() method
