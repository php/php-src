--TEST--
flock() tests
--FILE--
<?php

$file = __DIR__."/flock.dat";

$fp = fopen($file, "w");
fclose($fp);

try {
    var_dump(flock($fp, LOCK_SH|LOCK_NB));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

$fp = fopen($file, "w");

var_dump(flock($fp, LOCK_SH|LOCK_NB));
var_dump(flock($fp, LOCK_UN));
var_dump(flock($fp, LOCK_EX));
var_dump(flock($fp, LOCK_UN));

$would = array(1,2,3);
var_dump(flock($fp, LOCK_SH|LOCK_NB, $would));
var_dump($would);
var_dump(flock($fp, LOCK_UN, $would));
var_dump($would);
var_dump(flock($fp, LOCK_EX, $would));
var_dump($would);
var_dump(flock($fp, LOCK_UN, $would));
var_dump($would);

var_dump(flock($fp, -1));

try {
    var_dump(flock($fp, 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--CLEAN--
<?php
$file = __DIR__."/flock.dat";
unlink($file);
?>
--EXPECT--
flock(): supplied resource is not a valid stream resource
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
int(0)
bool(true)
int(0)
bool(true)
int(0)
bool(true)
flock(): Argument #2 ($operation) must be one of LOCK_SH, LOCK_EX, or LOCK_UN
