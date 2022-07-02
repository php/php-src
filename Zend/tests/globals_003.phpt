--TEST--
globals in local scope - 2
--INI--
variables_order="egpcs"
--FILE--
<?php

class test {

    static function bar() {

        var_dump(isset($_SERVER));
        var_dump(empty($_SERVER));
        var_dump(gettype($_SERVER));
        var_dump(count($_SERVER));

        var_dump($_SERVER['PHP_SELF']);
        unset($_SERVER['PHP_SELF']);
        var_dump($_SERVER['PHP_SELF']);

        unset($_SERVER);
        var_dump($_SERVER);

    }
}

test::bar();

echo "Done\n";
?>
--EXPECTF--
bool(true)
bool(false)
string(5) "array"
int(%d)
string(%d) "%s"

Warning: Undefined array key "PHP_SELF" in %s on line %d
NULL

Warning: Undefined global variable $_SERVER in %s on line %d
NULL
Done
