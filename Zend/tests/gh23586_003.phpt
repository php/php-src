--TEST--
GH-23586 (GC traversal storage is released when a destructor bails out)
--SKIPIF--
<?php
if (getenv('USE_ZEND_ALLOC') === '0') {
    die('skip Zend MM required');
}
?>
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
class Node {
    public $children = [];

    public function __destruct() {
        trigger_error('Destructor bailout', E_USER_ERROR);
    }
}

gc_disable();
$root = new Node();
for ($i = 0; $i < 2000; $i++) {
    $root->children[] = (object) ['parent' => $root];
}
unset($root);
gc_collect_cycles();
?>
--EXPECTF--
Fatal error: Destructor bailout in %s on line %d
