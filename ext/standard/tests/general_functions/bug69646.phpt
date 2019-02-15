--TEST--
Bug #69646 OS command injection vulnerability in escapeshellarg()
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN" )
  die("skip.. Windows only");
?>
--FILE--
<?php

$a = 'a\\';
$b = 'b -c d\\';
var_dump( $a, escapeshellarg($a) );
var_dump( $b, escapeshellarg($b) );

$helper_script = <<<SCRIPT
<?php

print( "--- ARG INFO ---\n" );
var_dump( \$argv );

SCRIPT;

$script = dirname(__FILE__) . DIRECTORY_SEPARATOR . "arginfo.php";
file_put_contents($script, $helper_script);

$cmd =  PHP_BINARY . " " . $script . " "  . escapeshellarg($a) . " " . escapeshellarg($b);

system($cmd);

unlink($script);
?>
--EXPECTF--
string(2) "a\"
string(5) ""a\\""
string(7) "b -c d\"
string(10) ""b -c d\\""
--- ARG INFO ---
array(3) {
  [0]=>
  string(%d) "%sarginfo.php"
  [1]=>
  string(2) "a\"
  [2]=>
  string(7) "b -c d\"
}
