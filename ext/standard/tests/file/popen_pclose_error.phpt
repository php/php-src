--TEST--
Test popen() and pclose function: error conditions
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
echo "*** Testing for error conditions ***\n";

try {
    popen("abc.txt", "rw");   // Invalid mode Argument
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

$file_handle = fopen($file_path."/popen.tmp", "w");
fclose($file_handle);
echo "\n--- Done ---";

?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/popen.tmp");
?>
--EXPECT--
*** Testing for error conditions ***
popen(): Argument #2 ($mode) must be either "r" or "w"

--- Done ---
