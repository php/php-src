--TEST--
Bug #31754 (dbase_open() fails for mode = 1)
--SKIPIF--
<?php
if (!extension_loaded('dbase')) {
	die('skip dbase extension not available');
}
?>
--FILE--
<?php

// database "definition"
$def = array(
	array("foo", "L")
);

// creation
$dbh = dbase_create('/tmp/bug31754.dbf', array(array('foo', 'L')));
dbase_close($dbh);

$dbh = dbase_open('/tmp/bug31754.dbf', 1);
unlink('/tmp/bug31754.dbf');

?>
--EXPECTF--
Warning: dbase_open(): Cannot open /tmp/bug31754.dbf in write-only mode in %sbug31754.php on line %d
