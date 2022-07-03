--TEST--
testing @ and error_reporting - 9
--FILE--
<?php

error_reporting(E_ALL & ~E_DEPRECATED);

function bar() {
    echo @$blah;
    echo $undef2;
}

function foo() {
    echo @$undef;
    error_reporting(E_ALL);
    echo $blah;
    return bar();
}

@foo();

var_dump(error_reporting());

echo "Done\n";
?>
--EXPECTF--
Warning: Undefined variable $blah (This will become an error in PHP 9.0) in %s on line %d

Warning: Undefined variable $undef2 (This will become an error in PHP 9.0) in %s on line %d
int(32767)
Done
