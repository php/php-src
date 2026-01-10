--TEST--
zstr_init_literal
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(strlen(zend_test_zstr_init_literal()), bin2hex(zend_test_zstr_init_literal()));

?>
--EXPECT--
int(7)
string(14) "666f6f00626172"
