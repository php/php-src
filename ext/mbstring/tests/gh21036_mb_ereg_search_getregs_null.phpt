--TEST--
GH-21036 (mb_ereg_search_getregs() crash after mb_eregi() invalidates search_re)
--CREDITS--
vi3tL0u1s
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_ereg_search_init")) die("skip mb_ereg_search_init() not available");
?>
--FILE--
<?php
// mb_eregi() can invalidate MBREX(search_re) via regex cache eviction
// while MBREX(search_regs) remains valid, causing NULL pointer dereference

mb_ereg_search_init("a", "a");
mb_ereg_search_pos();
mb_eregi("a", "a");  // This invalidates search_re
$result = mb_ereg_search_getregs();  // Should not crash
var_dump($result);
?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "a"
}
