--TEST--
basename bug #66395
--SKIPIF--
<?php if (substr(PHP_OS, 0, 3) != 'WIN') { die('skip Windows only basename tests'); } ?>
--FILE--
<?php
echo basename("c:file.txt") . "\n";
echo basename("d:subdir\\file.txt") . "\n";
echo basename("y:file.txt", ".txt") . "\n";
echo basename("notdriveletter:file.txt") . "\n";
?>
==DONE==
--EXPECT--
file.txt
file.txt
file
notdriveletter:file.txt
==DONE==
