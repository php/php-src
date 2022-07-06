--TEST--
Bug #62262: RecursiveArrayIterator does not implement Countable
--FILE--
<?php

var_dump(new RecursiveArrayIterator(array()) instanceof Countable);

?>
--EXPECT--
bool(true)
