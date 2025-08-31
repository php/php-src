--TEST--
Bug #75193 segfault in collator_convert_object_to_string
--EXTENSIONS--
intl
--FILE--
<?php
$a = new \Collator('en_US');
$b = [new stdclass, new stdclass];
var_dump($a->sort($b));
?>
--EXPECT--
bool(true)
