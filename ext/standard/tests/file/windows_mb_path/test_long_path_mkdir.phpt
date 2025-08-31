--TEST--
Mkdir with path length < 260 and > 248 has be a long path
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();

$start = realpath(__DIR__);
if (strlen($start) > 260 || strlen($start) > 248) {
    die("skip the starting path length is unsuitable for this test");
}

?>
--FILE--
<?php

$p = "";
$s = str_repeat('a', 50);
$how_many = 32;

for ($i = 0; $i < $how_many; $i++) {
    $p .= "$s\\";
}

$start = realpath(__DIR__);
$newstart = false;
if (strlen($start) <= 248) {
    // create the exact length
    $start = $start . "\\" . str_repeat('a', 251 - strlen($start) - 1);
    $newstart = true;
}

var_dump($start);
$p = $start . "\\" . $p;

var_dump($p);
var_dump(mkdir($p, 0777, true));
var_dump(file_exists($p));

$p7 = $p . "hello.txt";

var_dump(file_put_contents($p7, "hello"));
var_dump(file_get_contents($p7));

// cleanup
unlink($p7);
for ($i = 0; $i < $how_many; $i++) {
    $p0 = substr($p, 0, strlen($p) - $i*51);
    rmdir($p0);
}
if ($newstart) {
    rmdir($start);
}

?>
--EXPECTF--
string(251) "%s"
string(1884) "%s"
bool(true)
bool(true)
int(5)
string(5) "hello"
