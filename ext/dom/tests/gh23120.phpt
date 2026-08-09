--TEST--
GH-23120 (Stack overflow when comparing deeply nested DOM nodes)
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
function create_deep_document(): DOMDocument {
    $doc = new DOMDocument();
    $node = $doc->createElement('leaf', 'x');

    for ($i = 0; $i < 10000; $i++) {
        $parent = $doc->createElement('a');
        $parent->appendChild($node);
        $node = $parent;
    }

    $doc->appendChild($node);
    return $doc;
}

$doc1 = create_deep_document();
$doc2 = create_deep_document();

try {
    var_dump($doc1->documentElement->isEqualNode($doc2->documentElement));
} catch (\Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Maximum call stack size reached.
