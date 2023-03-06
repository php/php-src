--TEST--
Test disk_free_space and its alias diskfreespace() functions : basic functionality
--SKIPIF--
<?php
if (getenv("TRAVIS") === "true") die("skip inaccurate on TravisCI");
if (getenv('CIRRUS_CI')) die('skip Inaccurate on Cirrus');
?>
--INI--
memory_limit=32M
--FILE--
<?php
$file_path = __DIR__;

echo "*** Testing with existing directory ***\n";
var_dump( disk_free_space($file_path) );
var_dump( diskfreespace($file_path) );

echo "*** Testing with newly created directory ***\n";
$dir = "/disk_free_space";
mkdir($file_path.$dir);
echo" \n Free Space before writing to a file\n";
$space1 =  disk_free_space($file_path.$dir);
var_dump( $space1 );

$fh = fopen($file_path.$dir."/disk_free_space.tmp", "a");
$data = str_repeat("x", 0xffff);
fwrite($fh, $data);
fclose($fh);

echo "\n Free Space after writing to a file\n";
$space2 =  disk_free_space($file_path.$dir);
var_dump( $space2 );

if($space1 > $space2 )
  echo "\n Free Space Value Is Correct\n";
else {
  echo "\n Free Space Value Is Incorrect\n";
  var_dump($space1, $space2);
}

echo "*** Testing with Binary Input ***\n";
var_dump( disk_free_space(b"$file_path") );

echo"\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/disk_free_space/disk_free_space.tmp");
rmdir($file_path."/disk_free_space");
?>
--EXPECTF--
*** Testing with existing directory ***
float(%f)
float(%f)
*** Testing with newly created directory ***
 
 Free Space before writing to a file
float(%f)

 Free Space after writing to a file
float(%f)

 Free Space Value Is Correct
*** Testing with Binary Input ***
float(%f)

--- Done ---
