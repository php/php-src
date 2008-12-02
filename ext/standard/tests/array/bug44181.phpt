--TEST--
Bug #44181 (extract EXTR_OVERWRITE|EXTR_REFS can fail to create references)
--FILE--
<?php
$a = array('foo' => 'original.foo');

$foo = 'test';
$ref = &$a;

extract($a, EXTR_OVERWRITE|EXTR_REFS);
$foo = 'changed.foo';

var_dump($a['foo']);
echo "Done\n";
?>
--EXPECTF--
string(%d) "changed.foo"
Done
