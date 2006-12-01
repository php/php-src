--TEST--
dbase_open() tests
--SKIPIF--
<?php if (!extension_loaded("dbase")) print "skip"; ?>
--FILE--
<?php

$file = dirname(__FILE__)."/002.dbf";
@unlink($file);

$fp = fopen($file, "w");
fclose($fp);

var_dump(dbase_open($file, -1));
var_dump(dbase_open($file, 1000));
var_dump(dbase_open($file, 0));
var_dump(dbase_open($file."nonex", 0));
var_dump(dbase_open("", 0));

@unlink($file);

$def = array(
  array("date",    "D"),
  array("name",    "C",  50),
  array("age",      "N",  3, 0),
  array("email",    "C", 128),
  array("ismember", "L")
);

var_dump(dbase_create($file, $def));
var_dump(dbase_open($file, 0));

@unlink($file);

echo "Done\n";
?>
--EXPECTF--	
Warning: dbase_open(): Invalid access mode -1 in %s on line %d
bool(false)

Warning: dbase_open(): Invalid access mode 1000 in %s on line %d
bool(false)

Warning: dbase_open(): unable to open database %s in %s on line %d
bool(false)

Warning: dbase_open(): unable to open database %s in %s on line %d
bool(false)

Warning: dbase_open(): The filename cannot be empty. in %s on line %d
bool(false)
int(%d)
int(%d)
Done
