--TEST--
Test popen() and pclose function
--SKIPIF--
<?php
if (strtoupper( substr(PHP_OS, 0, 3) ) == 'SUN')
  die("skip Not valid for Sun Solaris");

if (strtoupper( substr(PHP_OS, 0, 3) ) == 'WIN')
  die("skip Not valid for Windows");
?>
--FILE--
<?php
$file_path = __DIR__;

$file_handle = fopen($file_path."/popen.tmp", "w");
fclose($file_handle);

$file_handle = fopen($file_path."/popen.tmp", "wb");
fclose($file_handle);
echo "--- Done ---";

?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/popen.tmp");
?>
--EXPECT--
--- Done ---
