--TEST--
fscanf() tests
--FILE--
<?php

$filename = __DIR__."/fscanf.dat";

file_put_contents($filename, "data");

$fp = fopen($filename, "rt");
var_dump(fscanf($fp, "%d", $v));
var_dump($v);
fclose($fp);

$fp = fopen($filename, "rt");
var_dump(fscanf($fp, "%s", $v));
var_dump($v);
fclose($fp);

$fp = fopen($filename, "rt");
try {
    fscanf($fp, "%s", $v, $v1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
var_dump($v);
var_dump($v1);
fclose($fp);

$v = array();
$v1 = array();
$fp = fopen($filename, "rt");
try {
    fscanf($fp, "", $v, $v1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
var_dump($v);
var_dump($v1);
fclose($fp);

$v = array();
$v1 = array();
$fp = fopen($filename, "rt");
try {
    fscanf($fp, "%.a", $v, $v1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
var_dump($v);
var_dump($v1);
fclose($fp);

@unlink($filename);
touch($filename);

$fp = fopen($filename, "rt");
var_dump(fscanf($fp, "%s", $v));
var_dump($v);
fclose($fp);

file_put_contents($filename, "data");

$fp = fopen($filename, "rt");
try {
    var_dump(fscanf($fp, "%s%d", $v));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--CLEAN--
<?php
$filename = __DIR__."/fscanf.dat";
unlink($filename);
?>
--EXPECT--
int(0)
NULL
int(1)
string(4) "data"
Variable is not assigned by any conversion specifiers
string(4) "data"
NULL
Variable is not assigned by any conversion specifiers
array(0) {
}
array(0) {
}
Bad scan conversion character "."
array(0) {
}
array(0) {
}
bool(false)
array(0) {
}
Different numbers of variable names and field specifiers
Done
