--TEST--
Test popen() and pclose function: basic functionality
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == 'WIN' )
  die("skip Not Valid for Windows");
?>
--FILE--
<?php
$file_path = __DIR__;
require($file_path."/file.inc");

echo "*** Testing popen() and pclose() with different processes ***\n";

echo "-- Testing popen(): reading from the pipe --\n";
$dirpath = $file_path."/popen_basic";
mkdir($dirpath);
touch($dirpath."/popen_basic.tmp");
define('CMD', "ls " . escapeshellarg($dirpath));
$file_handle = popen(CMD, 'r');
fpassthru($file_handle);
pclose($file_handle);

echo "-- Testing popen(): reading from a file using 'cat' command --\n";
create_files($dirpath, 1, "text_with_new_line", 0755, 100, "w", "popen_basic", 1, "bytes");
$filename = $dirpath."/popen_basic1.tmp";
$filename_escaped = escapeshellarg($filename);
$command = "cat $filename_escaped";
$file_handle = popen($command, "r");
$return_value =  fpassthru($file_handle);
echo "\n";
var_dump($return_value);
pclose($file_handle);
delete_files($dirpath, 1);

echo "*** Testing popen(): writing to the pipe ***\n";
$arr = array("ggg", "ddd", "aaa", "sss");
$file_handle = popen("sort", "w");
$counter = 0;
$newline = "\n";
foreach($arr as $str) {
  fwrite($file_handle, $str);
  fwrite($file_handle, $newline);
}
pclose($file_handle);


echo "*** Testing for return type of popen() and pclose() functions ***\n";
$string = "Test String";
$return_value_popen = popen("echo $string", "r");
var_dump( is_resource($return_value_popen) );
fpassthru($return_value_popen);
$return_value_pclose = pclose($return_value_popen);
var_dump( is_int($return_value_pclose) );

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$dirpath = $file_path."/popen_basic";
unlink($dirpath."/popen_basic.tmp");
unlink($dirpath."/popen_basic1.tmp");
rmdir($dirpath);
?>
--EXPECT--
*** Testing popen() and pclose() with different processes ***
-- Testing popen(): reading from the pipe --
popen_basic.tmp
-- Testing popen(): reading from a file using 'cat' command --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
*** Testing popen(): writing to the pipe ***
aaa
ddd
ggg
sss
*** Testing for return type of popen() and pclose() functions ***
bool(true)
Test String
bool(true)

--- Done ---
