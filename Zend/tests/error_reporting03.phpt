--TEST--
testing @ and error_reporting - 3
--FILE--
<?php

error_reporting(E_ALL & ~E_DEPRECATED);

function foo($arg) {
    echo @$nonex_foo;
}

function bar() {
    echo @$nonex_bar;
    throw new Exception("test");
}

function foo1() {
    echo $undef1;
    error_reporting(E_ALL);
    echo $undef2;
}

try {
    @foo(@bar(@foo1()));
} catch (Exception $e) {
}

var_dump(error_reporting());

echo "Done\n";
?>
--EXPECTF--
Warning: Undefined variable $undef2 in %s on line %d
int(32767)
Done
