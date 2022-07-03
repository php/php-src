--TEST--
testing @ and error_reporting - 4
--FILE--
<?php

error_reporting(E_ALL & ~E_DEPRECATED);

function foo() {
    echo $undef;
    error_reporting(E_ALL);
}


foo(@$var);

var_dump(error_reporting());

echo "Done\n";
?>
--EXPECTF--
Warning: Undefined variable $undef (This will become an error in PHP 9.0) in %s on line %d
int(32767)
Done
