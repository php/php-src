--TEST--
Test rename() function: basic functionality
--FILE--
<?php
/* Prototype: bool rename ( string $oldname, string $newname [, resource $context] );
   Description: Renames a file or directory
*/

echo "*** Testing rename() on non-existing file ***\n";
$file_path = dirname(__FILE__);
$src_name = "$file_path/rename_basic.tmp";
$dest_name = "$file_path/rename_basic_new.tmp";

// create the file
$fp = fopen($src_name, "w");
$s1 = stat($src_name);
fclose($fp);

var_dump( rename($src_name, $dest_name) ); // expecting true
var_dump( file_exists($src_name) ); // expecting false
var_dump( file_exists($dest_name) ); // expecting true

$s2 = stat("$file_path/rename_basic_new.tmp");

// checking statistics of old and renamed file - both should be same
for ($i = 0; $i <= 12; $i++) {
  if ($s1[$i] != $s2[$i]) {
    echo "rename_basic.tmp and rename_basic_new.tmp stat differ at element $i\n";
  }
}

echo "Done\n";
?>
--CLEAN--
<?php
unlink(dirname(__FILE__)."/rename_basic.tmp");
unlink(dirname(__FILE__)."/rename_basic_new.tmp");
?>
--EXPECTF--
*** Testing rename() on non-existing file ***
bool(true)
bool(false)
bool(true)
Done

