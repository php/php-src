--TEST--
executing a file with -F 
--SKIPIF--
<?php 
include "skipif.inc"; 
if (substr(PHP_OS, 0, 3) == 'WIN') {
	die ("skip not for Windows");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

$filename = dirname(__FILE__)."/010.test.php";
$filename_txt = dirname(__FILE__)."/010.test.txt";

$code = '
<?php
var_dump(fread(STDIN, 10));
?>
';

file_put_contents($filename, $code);

$txt = '
test
hello
';

file_put_contents($filename_txt, $txt);

var_dump(`cat "$filename_txt" | "$php" -n -F "$filename"`);

@unlink($filename);
@unlink($filename_txt);

echo "Done\n";
?>
--EXPECTF--	
string(39) "
string(10) "test
hello"

string(0) ""
"
Done
