--TEST--
Bug #45575 (Segfault with invalid non-string as event handler callback)
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

$db = ibase_connect($test_base);

function foobar($var) { var_dump($var); return true; }

ibase_set_event_handler($db, null, 'TEST1');
ibase_set_event_handler($db, 1, 'TEST1');
ibase_set_event_handler('foobar', 'TEST1');

?>
--EXPECTF--
Warning: ibase_set_event_handler(): Callback argument  is not a callable function in %s on line %d

Warning: ibase_set_event_handler(): Callback argument 1 is not a callable function in %s on line %d
