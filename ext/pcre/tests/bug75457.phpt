--TEST--
Bug #75457 (heap-use-after-free in php7.0.25)
--FILE--
<?php
$pattern = "/(((?(?C)0?=))(?!()0|.(?0)0)())/";
var_dump(preg_match($pattern, "hello"));
?>
--EXPECTF--
Warning: preg_match(): Compilation failed: assertion expected after (?( or (?(?C) at offset 8 in %sbug75457.php on line %d
bool(false)
