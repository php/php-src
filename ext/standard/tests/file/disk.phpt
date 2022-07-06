--TEST--
disk_total_space() and disk_free_space() tests
--INI--
precision=14
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip not for Windows');
}
?>
--CONFLICTS--
disk_total_space
--FILE--
<?php

var_dump(disk_free_space(-1));
var_dump(disk_total_space(-1));

var_dump(disk_free_space("/"));
var_dump(disk_total_space("/"));

var_dump(disk_free_space("/some/path/here"));
var_dump(disk_total_space("/some/path/here"));

echo "Done\n";
?>
--EXPECTF--
Warning: disk_free_space(): No such file or directory in %s on line %d
bool(false)

Warning: disk_total_space(): No such file or directory in %s on line %d
bool(false)
float(%f)
float(%f)

Warning: disk_free_space(): No such file or directory in %s on line %d
bool(false)

Warning: disk_total_space(): No such file or directory in %s on line %d
bool(false)
Done
