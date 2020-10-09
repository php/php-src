--TEST--
Match expression with trailing comma in condition list
--FILE--
<?php

function print_bool($bool) {
    echo match ($bool) {
        false,
        0,
            => "false\n",
        true,
        1,
            => "true\n",
        default,
            => "not bool\n",
    };
}

print_bool(false);
print_bool(0);
print_bool(true);
print_bool(1);
print_bool(2);
print_bool('foo');

?>
--EXPECT--
false
false
true
true
not bool
not bool
