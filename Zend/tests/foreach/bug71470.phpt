--TEST--
Bug #71470: Leaked 1 hashtable iterators
--FILE--
<?php

$array = [1, 2, 3];
foreach ($array as &$v) {
    die("foo\n");
}

?>
--EXPECT--
foo
