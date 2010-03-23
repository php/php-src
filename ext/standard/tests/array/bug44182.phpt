--TEST--
Bug #44182 (extract EXTR_REFS can fail to split copy-on-write references)
--FILE--
<?php
$a = array('foo' => 'original.foo');

$nonref = $a['foo'];
$ref = &$a;

extract($a, EXTR_REFS);
$a['foo'] = 'changed.foo';

var_dump($nonref);
echo "Done\n";
?>
--EXPECTF--
string(%d) "original.foo"
Done

