--TEST--
executing a code with -R
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

$filename_txt = dirname(__FILE__)."/010.test.txt";

$txt = '
test
hello
';

file_put_contents($filename_txt, $txt);

var_dump(`cat "$filename_txt" | "$php" -n -R "var_dump(1);"`);

@unlink($filename_txt);

echo "Done\n";
?>
--EXPECTF--	
string(21) "int(1)
int(1)
int(1)
"
Done
