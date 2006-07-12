--TEST--
dbase_create() tests
--SKIPIF--
<?php if (!extension_loaded("dbase")) print "skip"; ?>
--FILE--
<?php

function my_error_handler($errno, $errstr, $errfile, $errline) {
	var_dump($errstr);
}

set_error_handler('my_error_handler');

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
string(38) "dbase_create(): unknown field type 'E'"
bool(false)
string(38) "dbase_create(): unknown field type '-'"
bool(false)
int(%d)
string(71) "dbase_create(): expected field name as first element of list in field 0"
bool(false)
int(%d)
string(67) "Argument 2 passed to dbase_create() must be an array, integer given"
string(50) "dbase_create(): Expected array as second parameter"
bool(false)
string(%d) "unlink(%s001.dbf): No such file or directory"
string(66) "Argument 2 passed to dbase_create() must be an array, string given"
string(50) "dbase_create(): Expected array as second parameter"
bool(false)
Done
