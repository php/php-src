--TEST--
References are not counted twice
--FILE--
<?php

$ref1 = 1;
$ref2 = 2;

$arr = [&$ref1, &$ref1, &$ref2, &$ref2];
var_dump(serialize($arr));

?>
--EXPECT--
string(38) "a:4:{i:0;i:1;i:1;R:2;i:2;i:2;i:3;R:3;}"
