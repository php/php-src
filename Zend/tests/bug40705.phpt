--TEST--
Bug #40705 (Iterating within function moves original array pointer)
--FILE--
<?php
function doForeach($array)
{
    foreach ($array as $k => $v) {
        // do stuff
    }
}

$foo = array('foo', 'bar', 'baz');
var_dump(key($foo));
doForeach($foo);
var_dump(key($foo));
foreach ($foo as $k => $v) {
	var_dump($k);
}
var_dump(key($foo));
--EXPECT--
int(0)
int(0)
int(0)
int(1)
int(2)
NULL
