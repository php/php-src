--TEST--
Test closedir() function : usage variations - close directory handle twice
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/* Prototype  : void closedir([resource $dir_handle])
 * Description: Close directory connection identified by the dir_handle
 * Source code: ext/standard/dir.c
 * Alias to functions: close
 */

/*
 * close the directory handle twice using closedir() to test behaviour
 */

echo "*** Testing closedir() : usage variations ***\n";

//create temporary directory for test, removed in CLEAN section
$directory = __DIR__ . "/私はガラスを食べられますclosedir_variation2";
mkdir($directory);

$dh = opendir($directory);

echo "\n-- Close directory handle first time: --\n";
var_dump(closedir($dh));
echo "Directory Handle: ";
var_dump($dh);

echo "\n-- Close directory handle second time: --\n";
var_dump(closedir($dh));
echo "Directory Handle: ";
var_dump($dh);
?>
===DONE===
--CLEAN--
<?php
$directory = __DIR__ . "/私はガラスを食べられますclosedir_variation2";
rmdir($directory);
?>
--EXPECTF--
*** Testing closedir() : usage variations ***

-- Close directory handle first time: --
NULL
Directory Handle: resource(%d) of type (Unknown)

-- Close directory handle second time: --

Warning: closedir(): %s is not a valid Directory resource in %s on line %d
bool(false)
Directory Handle: resource(%d) of type (Unknown)
===DONE===
