--TEST--
Bug #43808 (date_create never fails (even when it should))
--FILE--
<?php
$date = date_create('asdfasdf');

if ($date instanceof DateTime) {
	echo "this is wrong, should be bool";
}

var_dump( $date );
?>
--EXPECT--
bool(false)
