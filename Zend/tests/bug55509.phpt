--TEST--
Bug #55509 (segfault on x86_64 using more than 2G memory)
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) {
  die('skip Not for 32-bits OS');
}
?>
--INI--
memory_limit=3G
--FILE--
<?php
$a1 = str_repeat("1", 1024 * 1024 * 1024 * 0.5);
echo "1\n";
$a2 = str_repeat("2", 1024 * 1024 * 1024 * 0.5);
echo "2\n";
$a3 = str_repeat("3", 1024 * 1024 * 1024 * 0.5);
echo "3\n";
$a4 = str_repeat("4", 1024 * 1024 * 1024 * 0.5);
echo "4\n";
$a5 = str_repeat("5", 1024 * 1024 * 1024 * 0.5);
echo "5\n";
$a6 = str_repeat("6", 1024 * 1024 * 1024 * 0.5);
echo "6\n";
?>
--EXPECTF--
1
2
3
4
5

Fatal error: Allowed memory size of %d bytes exhausted (tried to allocate %d bytes) in %s/bug55509.php on line %d
