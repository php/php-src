--TEST--
Bug #75193 segfault in collator_convert_object_to_string
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
$a = new \Collator('en_US');
$b = [new stdclass, new stdclass];
var_dump($a->sort($b));
?>
===DONE===
--EXPECT--
bool(true)
===DONE===
