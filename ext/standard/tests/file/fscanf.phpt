--TEST--
fscanf() tests
--FILE--
<?php

$filename = dirname(__FILE__)."/fscanf.dat";

var_dump(fscanf());
var_dump(fscanf(array()));
var_dump(fscanf(array(), array(), new stdclass));

file_put_contents($filename, "data");

$fp = fopen($filename, "r");
var_dump(fscanf($fp, "%d", $v));
var_dump($v);
fclose($fp);

$fp = fopen($filename, "r");
var_dump(fscanf($fp, "%s", $v));
var_dump($v);
fclose($fp);

$fp = fopen($filename, "r");
var_dump(fscanf($fp, "%s", $v, $v1));
var_dump($v);
var_dump($v1);
fclose($fp);

$v = array();
$v1 = array();
$fp = fopen($filename, "r");
var_dump(fscanf($fp, "", $v, $v1));
var_dump($v);
var_dump($v1);
fclose($fp);

$v = array();
$v1 = array();
$fp = fopen($filename, "r");
var_dump(fscanf($fp, "%.a", $v, $v1));
var_dump($v);
var_dump($v1);
fclose($fp);

@unlink($filename);
touch($filename);

$fp = fopen($filename, "r");
var_dump(fscanf($fp, "%s", $v));
var_dump($v);
fclose($fp);

file_put_contents($filename, "data");

$fp = fopen($filename, "r");
var_dump(fscanf($fp, "%s%d", $v));

@unlink($filename);
echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for fscanf() in %s on line %d
NULL

Warning: Wrong parameter count for fscanf() in %s on line %d
NULL

Warning: fscanf(): supplied argument is not a valid File-Handle resource in %s on line %d
bool(false)
int(0)
NULL
int(1)
string(4) "data"

Warning: fscanf(): Variable is not assigned by any conversion specifiers in %s on line %d
int(-1)
string(4) "data"
NULL

Warning: fscanf(): Variable is not assigned by any conversion specifiers in %s on line %d
int(-1)
array(0) {
}
array(0) {
}

Warning: fscanf(): Bad scan conversion character "." in %s on line %d
int(-1)
array(0) {
}
array(0) {
}
bool(false)
array(0) {
}

Warning: fscanf(): Different numbers of variable names and field specifiers in %s on line %d
int(-1)
Done
