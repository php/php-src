--TEST--
Match expression default case
--FILE--
<?php

function get_value($i) {
    return match ($i) {
        1 => 1,
        2 => 2,
        default => 'default',
    };
}

echo get_value(0) . "\n";
echo get_value(1) . "\n";
echo get_value(2) . "\n";
echo get_value(3) . "\n";

?>
--EXPECT--
default
1
2
default
