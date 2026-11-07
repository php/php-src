--TEST--
Str\Matcher deduplication retains the same storage as a single keyword
--EXTENSIONS--
strmatch
--SKIPIF--
<?php
if (memory_get_usage(false) === 0) {
    die('skip Zend memory accounting is required');
}
?>
--FILE--
<?php
function retainedMemory(array $keywords): int {
    $before = memory_get_usage(false);
    $matcher = new Str\Matcher($keywords);
    $extra = memory_get_usage(false) - $before;
    unset($matcher);
    return $extra;
}
$single = retainedMemory(['needle']);
$duplicates = array_fill(0, 10000, 'needle');
var_dump(retainedMemory($duplicates) <= $single + 1024);
?>
--EXPECT--
bool(true)
