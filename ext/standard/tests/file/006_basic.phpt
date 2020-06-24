--TEST--
Test fileperms() & chmod() functions: basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not on Windows');
}
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
$path = __DIR__;

echo "*** Testing fileperms(), chmod() with files and dirs ***\n";
fopen($path."/perm.tmp", "w");
var_dump( chmod($path."/perm.tmp", 0755 ) );
printf("%o", fileperms($path."/perm.tmp") );
echo "\n";
clearstatcache();

mkdir($path."/perm");
var_dump( chmod( $path."/perm", 0777 ) );
printf("%o", fileperms($path."/perm") );
echo "\n";
clearstatcache();

echo "Done\n";
?>
--CLEAN--
<?php
unlink(__DIR__."/perm.tmp");
rmdir(__DIR__."/perm");
?>
--EXPECT--
*** Testing fileperms(), chmod() with files and dirs ***
bool(true)
100755
bool(true)
40777
Done
