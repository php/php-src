--TEST--
ibase_drop_db(): Basic test
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

unlink($file = tempnam('/tmp',"php_ibase_test"));


$db = ibase_query(IBASE_CREATE, 
		sprintf("CREATE SCHEMA '%s' USER '%s' PASSWORD '%s' DEFAULT CHARACTER SET %s",$file,
		$user, $password, ($charset = ini_get('ibase.default_charset')) ? $charset : 'NONE'));

var_dump($db);
var_dump(ibase_drop_db($db));
var_dump(ibase_drop_db(1));
var_dump(ibase_drop_db(NULL));

?>
--EXPECTF--
resource(%d) of type (Firebird/InterBase link)
bool(true)

Warning: ibase_drop_db() expects parameter 1 to be resource, integer given in %s on line %d
NULL

Warning: ibase_drop_db() expects parameter 1 to be resource, null given in %s on line %d
NULL
