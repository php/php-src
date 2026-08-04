--TEST--
GH-15911 (AppendIterator appended to itself overflows the C stack)
--SKIPIF--
<?php
if (!function_exists('zend_test_zend_call_stack_get')) die("skip zend_test_zend_call_stack_get() is not available");
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--EXTENSIONS--
zend_test
--INI--
memory_limit=2G
zend.max_allowed_stack_size=512K
--FILE--
<?php
$it = new AppendIterator();
$it->append($it);
foreach ($it as $v) {
}
?>
--EXPECTREGEX--
.*Maximum call stack size of \d+ bytes.*Infinite recursion\?.*thrown in .* on line \d+
