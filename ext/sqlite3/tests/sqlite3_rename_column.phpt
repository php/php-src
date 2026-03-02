--TEST--
SQLite3 - rename column while SQLite3Result is open
--EXTENSIONS--
sqlite3
--SKIPIF--
<?php
if (SQLite3::version()['versionNumber'] < 3025000) {
    die("skip: sqlite3 library version < 3.25: no support for rename column");
}
?>
--FILE--
<?php

$db = new SQLite3(':memory:');

$db->exec('CREATE TABLE tbl (orig text)');
$db->exec('insert into tbl values ("one"), ("two")');

$res1 = $db->prepare('select * from tbl')->execute();
$res2 = $db->prepare('select * from tbl')->execute();

var_dump(array_key_first($res1->fetchArray(SQLITE3_ASSOC)));
var_dump(array_key_first($res2->fetchArray(SQLITE3_ASSOC)));

$db->exec('alter table tbl rename column orig to changed');

$res1->reset();
var_dump(array_key_first($res1->fetchArray(SQLITE3_ASSOC)));
var_dump(array_key_first($res2->fetchArray(SQLITE3_ASSOC)));

?>
--EXPECT--
string(4) "orig"
string(4) "orig"
string(7) "changed"
string(4) "orig"
