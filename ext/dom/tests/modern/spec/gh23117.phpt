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
zend.max_allowed_stack_size=256K
--FILE--
<?php
// Build bottom-up so the insertion cycle-check stays O(1); top-down is O(n^2).
$doc = Dom\XMLDocument::createEmpty();
$root = $doc->createElement('root');
for ($s = 0; $s < 2; $s++) {
    $node = $doc->createElement('a');
    for ($i = 0; $i < 25000; $i++) {
        $parent = $doc->createElement('a');
        $parent->appendChild($node);
        $node = $parent;
    }
    $root->appendChild($node);
}
$doc->appendChild($root);

try {
    $doc->normalize();
} catch (\Error $e) {
    echo "normalize: ", $e::class, ": ", $e->getMessage(), "\n";
    var_dump($e->getPrevious());
}
?>
--EXPECT--
normalize: Error: Maximum call stack size reached. Infinite recursion?
NULL
