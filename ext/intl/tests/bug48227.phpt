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
NumberFormatter::format(): Argument #1 ($num) must be of type int|float, string given
string(1) "1"
string(1) "0"
NumberFormatter::format(): Argument #1 ($num) must be of type int|float, NumberFormatter given
