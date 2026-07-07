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
// Build bottom-up so the insertion cycle-check stays O(1); top-down is O(n^2).
$doc = Dom\XMLDocument::createEmpty();
$node = $doc->createElement('a');
for ($i = 0; $i < 10000; $i++) {
    $parent = $doc->createElement('a');
    $parent->appendChild($node);
    $node = $parent;
}
$doc->appendChild($node);

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
