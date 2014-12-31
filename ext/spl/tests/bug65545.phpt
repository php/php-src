--TEST--
SplFileObject::fread function
--FILE--
<?php
$obj = new SplFileObject(__FILE__, 'r');
$data = $obj->fread(5);
var_dump($data);

$data = $obj->fread();
var_dump($data);

$data = $obj->fread(0);
var_dump($data);

// read more data than is available
$data = $obj->fread(filesize(__FILE__) + 32);
var_dump(strlen($data) === filesize(__FILE__) - 5);

?>
--EXPECTF--
string(5) "<?php"

Warning: SplFileObject::fread() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: SplFileObject::fread(): Length parameter must be greater than 0 in %s on line %d
bool(false)
bool(true)
