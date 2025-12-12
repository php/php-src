--TEST--
GH-18753 (file_get_contents() and file_put_contents() fail with data >=2GB on macOS & BSD)
--CREDITS--
Gregory House <grzegorz129@gmail.com>
--INI--
memory_limit=-1
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Darwin' && PHP_OS_FAMILY !== 'BSD') die('skip is not macOS or BSD');
if (PHP_INT_SIZE !== 8) die("skip 64-bit only");
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
echo "-- file_put_contents() --\n";
echo file_put_contents('bigfile', str_repeat('a', 2 ** 31));

echo "\n-- file_get_contents() --\n";
echo strlen(file_get_contents('bigfile'));
?>
--CLEAN--
<?php
unlink('bigfile');
?>
--EXPECT--
-- file_put_contents() --
2147483648
-- file_get_contents() --
2147483648
