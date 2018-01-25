--TEST--
bzopen() and invalid parameters
--SKIPIF--
<?php if (!extension_loaded("bz2")) print "skip"; ?>
--FILE--
<?php

var_dump(bzopen());
var_dump(bzopen("", ""));
var_dump(bzopen("", "r"));
var_dump(bzopen("", "w"));
var_dump(bzopen("", "x"));
var_dump(bzopen("", "rw"));
var_dump(bzopen("no_such_file", "r"));

$fp = fopen(__FILE__,"r");
var_dump(bzopen($fp, "r"));

echo "Done\n";
?>
--EXPECTF--
Warning: bzopen() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: bzopen(): '' is not a valid mode for bzopen(). Only 'w' and 'r' are supported. in %s on line %d
bool(false)

Warning: bzopen(): filename cannot be empty in %s on line %d
bool(false)

Warning: bzopen(): filename cannot be empty in %s on line %d
bool(false)

Warning: bzopen(): 'x' is not a valid mode for bzopen(). Only 'w' and 'r' are supported. in %s on line %d
bool(false)

Warning: bzopen(): 'rw' is not a valid mode for bzopen(). Only 'w' and 'r' are supported. in %s on line %d
bool(false)

Warning: bzopen(no_such_file): failed to open stream: No such file or directory in %s on line %d
bool(false)
resource(%d) of type (stream)
Done
