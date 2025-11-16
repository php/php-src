--TEST--
extract() with negative keys
--FILE--
<?php

$arr = [-1 => "foo"];
var_dump(extract($arr, EXTR_PREFIX_ALL | EXTR_REFS, "prefix"));

?>
--EXPECT--
int(0)
