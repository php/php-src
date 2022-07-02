--TEST--
Test fileperms() & chmod() functions: usage variation - misc. perms
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not on Windows');
}
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
/* Testing with miscellaneous Permission */

echo "*** Testing fileperms() & chmod() : usage variations ***\n";

$file_name = __DIR__."/006_variation2.tmp";
$file_handle = fopen($file_name, "w");
fclose($file_handle);
$dir_name = __DIR__."/006_variation2";
mkdir($dir_name);

echo "\n*** Testing fileperms(), chmod() with miscellaneous permissions ***\n";
$perms_array = array(
  /* testing sticky bit */
  07777,
  00000,
  01000,
  011111,
  /* negative  values as permission */
  -0777, // permissions will be given as 2's complement form of -0777
  -07777, // permissions will be given as 2's complement form of -07777

  /* decimal values as permission  */
  777, // permissions will be given as octal equivalent value of 777
  7777, // permissions will be given as octal equivalent value of 7777
  -7777, // permissions are given as 2's complement of octal equivalent of 7777

  /* hex value as permission */
  0x777, // permissions will be given as ocatal equivalent value of 0x777
  0x7777,

  /* strings notation of permission,  wont work properly */
  "r+w",
  "r+w+x",
  "u+rwx",
  "u+rwx, g+rw, o+wx"
);

$count = 1;
foreach($perms_array as $permission) {
  echo "-- Iteration $count --\n";
  try {
    var_dump( chmod($file_name, $permission) );
    printf("%o", fileperms($file_name) );
    echo "\n";
    clearstatcache();
  } catch (TypeError $e) {
    echo $e->getMessage(), "\n";
  }

  try {
    var_dump( chmod($dir_name, $permission) );
    printf("%o", fileperms($dir_name) );
    echo "\n";
    clearstatcache();
  } catch (TypeError $e) {
    echo $e->getMessage(), "\n";
  }
  $count++;
}
echo "*** Done ***\n";
?>
--CLEAN--
<?php
chmod(__DIR__."/006_variation2.tmp", 0777);
chmod(__DIR__."/006_variation2", 0777);
unlink(__DIR__."/006_variation2.tmp");
rmdir(__DIR__."/006_variation2");
?>
--EXPECT--
*** Testing fileperms() & chmod() : usage variations ***

*** Testing fileperms(), chmod() with miscellaneous permissions ***
-- Iteration 1 --
bool(true)
107777
bool(true)
47777
-- Iteration 2 --
bool(true)
100000
bool(true)
40000
-- Iteration 3 --
bool(true)
101000
bool(true)
41000
-- Iteration 4 --
bool(true)
101111
bool(true)
41111
-- Iteration 5 --
bool(true)
107001
bool(true)
47001
-- Iteration 6 --
bool(true)
100001
bool(true)
40001
-- Iteration 7 --
bool(true)
101411
bool(true)
41411
-- Iteration 8 --
bool(true)
107141
bool(true)
47141
-- Iteration 9 --
bool(true)
100637
bool(true)
40637
-- Iteration 10 --
bool(true)
103567
bool(true)
43567
-- Iteration 11 --
bool(true)
103567
bool(true)
43567
-- Iteration 12 --
chmod(): Argument #2 ($permissions) must be of type int, string given
chmod(): Argument #2 ($permissions) must be of type int, string given
-- Iteration 13 --
chmod(): Argument #2 ($permissions) must be of type int, string given
chmod(): Argument #2 ($permissions) must be of type int, string given
-- Iteration 14 --
chmod(): Argument #2 ($permissions) must be of type int, string given
chmod(): Argument #2 ($permissions) must be of type int, string given
-- Iteration 15 --
chmod(): Argument #2 ($permissions) must be of type int, string given
chmod(): Argument #2 ($permissions) must be of type int, string given
*** Done ***
