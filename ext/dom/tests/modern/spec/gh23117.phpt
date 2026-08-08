--TEST--
GH-23117 (Stack overflow when normalizing a deeply nested Dom\XMLDocument)
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
$doc = Dom\XMLDocument::createFromString(str_repeat('<a>', 100000) . 'x' . str_repeat('</a>', 100000), LIBXML_PARSEHUGE);

try {
    $doc->normalize();
} catch (\Error $e) {
    echo "normalize: ", $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
normalize: Error: Maximum call stack size reached. Infinite recursion?
