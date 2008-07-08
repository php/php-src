--TEST--
Closure 011: Lexical copies not static in closure
--SKIPIF--
<?php 
	if (!class_exists('Closure')) die('skip Closure support is needed');
?>
--FILE--
<?php
$i = 1;
$lambda = function () use ($i) {
    return ++$i;
};
$lambda();
echo $lambda()."\n";
//early prototypes gave 3 here because $i was static in $lambda
?>
--EXPECT--
2
