--TEST--
dbase_create() tests
--SKIPIF--
<?php if (!extension_loaded("dbase")) print "skip"; ?>
--FILE--
<?php

$fields_arr = Array(
    Array(
        array('date','D'),
        ),
    Array(
        array('error', 'E'),
        ),
    Array(
        array('error', -1),
        ),
    Array(
        array(-1, 'N', 3, 0),
        ),
    Array(
        array(),
        ),
    Array(
        ),
);

$file = dirname(__FILE__).'/001.dbf';

foreach ($fields_arr as $fields) {
    var_dump(dbase_create($file, $fields));
    @unlink($file);
}

var_dump(dbase_create($file, -1));
@unlink($file);

var_dump(dbase_create("", ""));

echo "Done\n";
?>
--EXPECTF--	
int(%d)

Warning: dbase_create(): unknown field type 'E' in %s on line %d
bool(false)

Warning: dbase_create(): unknown field type '-' in %s on line %d
bool(false)
int(%d)

Warning: dbase_create(): expected field name as first element of list in field 0 in %s on line %d
bool(false)

Warning: dbase_create(): Unable to create database without fields in %s on line %d
bool(false)

Warning: dbase_create(): Expected array as second parameter in %s on line %d
bool(false)

Warning: dbase_create(): Expected array as second parameter in %s on line %d
bool(false)
Done
