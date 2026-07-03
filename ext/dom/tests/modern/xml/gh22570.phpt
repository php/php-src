--TEST--
GH-22570 (Stack overflow when serializing a deeply nested Dom\XMLDocument)
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (ini_get('zend.max_allowed_stack_size') === false) {
    die('skip No stack limit support');
}
if (getenv('SKIP_ASAN')) {
    die('skip ASAN needs different stack limit setting due to more stack space usage');
}
?>
--INI--
zend.max_allowed_stack_size=512K
--FILE--
<?php
// Build via the DOM API, not the parser: libxml caps parse depth even with
// LIBXML_PARSEHUGE on some platforms; the serializer recursion is the bug.
$doc = Dom\XMLDocument::createEmpty();
$node = $doc->appendChild($doc->createElement('root'));
for ($i = 0; $i < 100000; $i++) {
    $node = $node->appendChild($doc->createElement('a'));
}

try {
    $doc->saveXml();
} catch (\Error $e) {
    echo "saveXml: ", $e::class, ": ", $e->getMessage(), "\n";
}

try {
    $doc->documentElement->innerHTML;
} catch (\Error $e) {
    echo "innerHTML: ", $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
saveXml: Error: Maximum call stack size reached. Infinite recursion?
innerHTML: Error: Maximum call stack size reached. Infinite recursion?
