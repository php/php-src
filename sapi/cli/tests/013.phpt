--TEST--
running PHP code before and after processing input lines with -B and -E
--SKIPIF--
<?php
include "skipif.inc";
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die ("skip not for Windows");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

$filename_txt = __DIR__."/013.test.txt";
$filename_txt_escaped = escapeshellarg($filename_txt);
file_put_contents($filename_txt, "test\nfile\ncontents\n");

var_dump(`cat $filename_txt_escaped | $php -n -B 'var_dump("start");'`);
var_dump(`cat $filename_txt_escaped | $php -n -E 'var_dump("end");'`);
var_dump(`cat $filename_txt_escaped | $php -n -B 'var_dump("start");' -E 'var_dump("end");'`);

@unlink($filename_txt);

echo "Done\n";
?>
--EXPECT--
string(18) "string(5) "start"
"
string(16) "string(3) "end"
"
string(34) "string(5) "start"
string(3) "end"
"
Done
