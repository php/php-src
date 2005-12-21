--TEST--
SPL: iterator_to_array() and iterator_count()
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

$it = new ArrayObject(array("x"=>1, 1=>2, 3=>3, 4, "1"=>5));

$ar = iterator_to_array($it);

var_dump(iterator_count($it));

print_r($ar);

foreach($ar as $v)
{
	var_dump($v);
}

?>
===DONE===
--EXPECT--
int(4)
Array
(
    [x] => 1
    [1] => 5
    [3] => 3
    [4] => 4
)
int(1)
int(5)
int(3)
int(4)
===DONE===
