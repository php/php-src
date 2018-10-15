--TEST--
touch() with times
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
?>
--FILE--
<?php

$filename = dirname(__FILE__)."/touch.dat";


var_dump(touch($filename, 101));
var_dump(filemtime($filename));
var_dump(fileatime($filename));

@unlink($filename);

@unlink($filename);
var_dump(touch($filename, 100, 102));
var_dump(filemtime($filename));
var_dump(fileatime($filename));

@unlink($filename);
echo "Done\n";

?>
--EXPECTF--
bool(true)
int(101)
int(101)
bool(true)
int(100)
int(102)
Done
