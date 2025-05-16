--TEST--
GH-18519: Nested object comparison leading to stack overflow
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('skip as it fatally crash');
?>
--FILE--
<?php

error_reporting(E_ALL & ~E_DEPRECATED);

class Node {
    public $next;
    // forcing dynamic property creation is key
}

$first = new Node();
$first->previous = $first;
$first->next = $first;

$cur = $first;

for ($i = 0; $i < 50000; $i++) {
    $new = new Node();
    $new->previous = $cur;
    $cur->next = $new;
    $new->next = $first;
    $first->previous = $new;
    $cur = $new;
}

try {
	// Force comparison manually to trigger zend_hash_compare
	$first == $cur;
} catch(Error $e) {
	echo $e->getMessage(). PHP_EOL;
}
?>
--EXPECTREGEX--
(Object compare - stack limit reached|Fatal error: Nesting level too deep - recursive dependency?.+)
