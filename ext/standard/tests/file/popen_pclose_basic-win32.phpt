--TEST--
Test popen() and pclose function: basic functionality
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' )
  die("skip Not Valid for Linux");
?>
--FILE--
<?php
/*
 *  Prototype: resource popen ( string command, string mode )
 *  Description: Opens process file pointer.

 *  Prototype: int pclose ( resource handle );
 *  Description: Closes process file pointer.
 */

echo "*** Testing popen(): reading from the pipe ***\n";

$file_path = dirname(__FILE__);

$string = "Sample String";
$file_handle = popen(" echo $string", "r");
fpassthru($file_handle);
pclose($file_handle);

echo "*** Testing popen(): writing to the pipe ***\n";
$arr = array("ggg", "ddd", "aaa", "sss");
// popen("sort", "w") fails if variables_order="GPCS"
// this is set in the default INI file
// it doesn't seem to be changeable in the --INI-- section
//	also, doing: ini_set('variables_order', ''); doesn't work!
//
// the only solution is to either put the absolute path here, or
// remove variables_order= from PHP.ini (setting it in run-test's
// default INI will fail too)
//
// since we can't depend on PHP.ini being set a certain way,
// have to put the absolute path here.

$sysroot = exec('echo %SYSTEMROOT%');

$file_handle = popen("$sysroot/system32/sort", "w");
$newline = "\n";
foreach($arr as $str) {
  fwrite($file_handle, (binary)$str);
  fwrite($file_handle, (binary)(binary)(binary)(binary)(binary)(binary)(binary)(binary)(binary)$newline);
}
pclose($file_handle);

echo "*** Testing popen() and pclose(): return type ***\n";
$return_value_popen = popen("echo $string", "r");
fpassthru($return_value_popen);
var_dump( is_resource($return_value_popen) );
$return_value_pclose = pclose($return_value_popen);
var_dump( is_int($return_value_pclose) );

echo "\n--- Done ---";
?>
--EXPECT--
*** Testing popen(): reading from the pipe ***
Sample String
*** Testing popen(): writing to the pipe ***
aaa
ddd
ggg
sss
*** Testing popen() and pclose(): return type ***
Sample String
bool(true)
bool(true)

--- Done ---
