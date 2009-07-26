--TEST--
Bug #48227 (NumberFormatter::format leaks memory)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

$x = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
var_dump($x->format(''));
var_dump($x->format(1));
var_dump($x->format(NULL));
var_dump($x->format($x));

?>
--EXPECTF--
string(1) "0"
string(1) "1"
string(1) "0"

Notice: Object of class NumberFormatter could not be converted to int in %s on line %d
string(1) "1"
