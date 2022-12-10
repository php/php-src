--TEST--
Test file_descriptor() function: basic functionality
--FILE--
<?php
echo "*** Testing fileno() : basic functionality ***\n";
$tmpFile1 = __DIR__ . '/fileno.tmp1';
$file = fopen($tmpFile1, 'wb');
fclose($file);

$file = fopen($tmpFile1, 'rb');
var_dump(file_descriptor($file));

var_dump(file_descriptor(STDOUT));

echo "Done";
?>
--CLEAN--
<?php
$tmpFile1 = __DIR__ . '/fileno.tmp1';
unlink($tmpFile1);
?>
--EXPECTF--
*** Testing fileno() : basic functionality ***
int(%d)
int(1)
Done
