--TEST--
Bug #48227 (NumberFormatter::format leaks memory)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

$x = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
foreach (['', 1, NULL, $x] as $value) {
    try {
        var_dump($x->format($value));
    } catch (TypeError $ex) {
        echo $ex->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
NumberFormatter::format() expects argument #1 ($value) to be of type number, string given
string(1) "1"
string(1) "0"
NumberFormatter::format() expects argument #1 ($value) to be of type number, object given
