--TEST--
Internal enum reassignment to constants preserves identity
--EXTENSIONS--
zend_test
--FILE--
<?php

const E = ZendTestStringBacked::FIRST;

var_dump(E);
var_dump(E === ZendTestStringBacked::FIRST);

?>
--EXPECT--
enum(ZendTestStringBacked::FIRST)
bool(true)
