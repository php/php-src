--TEST--
Test fileperms() & chmod() functions: usage variation - misc. perms
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not on Windows');
}
// Skip if being run by root
$filename = dirname(__FILE__)."/006_root_check.tmp";
$fp = fopen($filename, 'w');
fclose($fp);
if(fileowner($filename) == 0) {
        unlink ($filename);
        die('skip...cannot be run as root\n');
}

unlink($filename);

?>
--FILE--
<?php
/* 
  Prototype: int fileperms ( string $filename );
  Description: Returns the permissions on the file, or FALSE in case of an error

  Prototype: bool chmod ( string $filename, int $mode );
  Description: Attempts to change the mode of the file specified by 
               filename to that given in mode
*/

/* Testing with miscellaneous Permission */

echo "*** Testing fileperms() & chmod() : usage variations ***\n";

$file_name = dirname(__FILE__)."/006_variation2.tmp";
$file_handle = fopen($file_name, "w");
fclose($file_handle);
$dir_name = dirname(__FILE__)."/006_variation2";
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
  var_dump( chmod($file_name, $permission) );
  printf("%o", fileperms($file_name) );
  echo "\n";
  clearstatcache();
 
  var_dump( chmod($dir_name, $permission) );
  printf("%o", fileperms($dir_name) );
  echo "\n";
  clearstatcache();
  $count++;
}
echo "*** Done ***\n";
?>
--CLEAN--
<?php
chmod(dirname(__FILE__)."/006_variation2.tmp", 0777);
chmod(dirname(__FILE__)."/006_variation2", 0777);
unlink(dirname(__FILE__)."/006_variation2.tmp");
rmdir(dirname(__FILE__)."/006_variation2");
?>
--EXPECTF--	
*** Testing fileperms() & chmod() : usage variations ***

*** Testing fileperms(), chmod() with miscellaneous permissions ***
-- Iteration 1 --
bool(true)
%d
bool(true)
%d
-- Iteration 2 --
bool(true)
%d
bool(true)
%d
-- Iteration 3 --
bool(true)
%d
bool(true)
%d
-- Iteration 4 --
bool(true)
%d
bool(true)
%d
-- Iteration 5 --
bool(true)
%d
bool(true)
%d
-- Iteration 6 --
bool(true)
%d
bool(true)
%d
-- Iteration 7 --
bool(true)
%d
bool(true)
%d
-- Iteration 8 --
bool(true)
%d
bool(true)
%d
-- Iteration 9 --
bool(true)
%d
bool(true)
%d
-- Iteration 10 --
bool(true)
%d
bool(true)
%d
-- Iteration 11 --
bool(true)
%d
bool(true)
%d
-- Iteration 12 --

Warning: chmod() expects parameter 2 to be long, Unicode string given in %s on line %d
NULL
%d

Warning: chmod() expects parameter 2 to be long, Unicode string given in %s on line %d
NULL
%d
-- Iteration 13 --

Warning: chmod() expects parameter 2 to be long, Unicode string given in %s on line %d
NULL
%d

Warning: chmod() expects parameter 2 to be long, Unicode string given in %s on line %d
NULL
%d
-- Iteration 14 --

Warning: chmod() expects parameter 2 to be long, Unicode string given in %s on line %d
NULL
%d

Warning: chmod() expects parameter 2 to be long, Unicode string given in %s on line %d
NULL
%d
-- Iteration 15 --

Warning: chmod() expects parameter 2 to be long, Unicode string given in %s on line %d
NULL
%d

Warning: chmod() expects parameter 2 to be long, Unicode string given in %s on line %d
NULL
%d
*** Done ***

