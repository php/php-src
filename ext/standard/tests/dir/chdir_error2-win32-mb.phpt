--TEST--
Test chdir() function : error conditions - Non-existent directory
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/*
 * Pass a directory that does not exist as $directory to chdir() to test behaviour
 */

echo "*** Testing chdir() : error conditions ***\n";

$directory = __FILE__ . '/私はガラスを食べられますidonotexist';

var_dump(chdir($directory));
?>
--EXPECTF--
*** Testing chdir() : error conditions ***

Warning: chdir(): %s (errno %d) in %s on line %d
bool(false)
