--TEST--
Bug #46031 (Segfault in AppendIterator::next)
--FILE--
<?php
$x = new AppendIterator();
var_dump($x->next());
?>
--EXPECT--
NULL
