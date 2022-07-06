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

$filename = __DIR__."/010.test.php";
$filename_txt = __DIR__."/010.test.txt";

$code = '
<?php
var_dump(fread(STDIN, 10));
?>
';

file_put_contents($filename, $code);

$txt = '
test
hello';

file_put_contents($filename_txt, $txt);

var_dump(`cat "$filename_txt" | "$php" -n -F "$filename"`);

?>
--CLEAN--
<?php
@unlink(__DIR__."/010.test.php");
@unlink(__DIR__."/010.test.txt");
?>
--EXPECT--
string(25) "
string(10) "test
hello"
"
