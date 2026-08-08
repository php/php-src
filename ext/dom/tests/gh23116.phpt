--TEST--
GH-23116 (Stack overflow when normalizing a deeply nested DOMDocument)
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
$doc = new DOMDocument();
$doc->loadXML(str_repeat('<a>', 100000) . 'x' . str_repeat('</a>', 100000), LIBXML_PARSEHUGE);

try {
    $doc->normalize();
} catch (\Error $e) {
    echo "normalize: ", $e::class, ": ", $e->getMessage(), "\n";
}

try {
    $doc->normalizeDocument();
} catch (\Error $e) {
    echo "normalizeDocument: ", $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
normalize: Error: Maximum call stack size reached. Infinite recursion?
normalizeDocument: Error: Maximum call stack size reached. Infinite recursion?
