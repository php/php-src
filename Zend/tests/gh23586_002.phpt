--TEST--
GH-23586 (GC can finish a wide traversal at the memory limit)
--SKIPIF--
<?php
if (getenv('USE_ZEND_ALLOC') === '0') {
    die('skip Zend MM required');
}
?>
--INI--
memory_limit=256M
--FILE--
<?php
gc_disable();
$root = new stdClass();
for ($i = 0; $i < 300000; $i++) {
    $root->children[] = (object) ['parent' => $root];
}
unset($root);
ini_set('memory_limit', (string) memory_get_usage(true));
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(300002)
