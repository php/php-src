--TEST--
Match expression exception on unhandled case
--FILE--
<?php

function get_value($i) {
    return match ($i) {
        1 => 1,
        2 => 2,
    };
}

echo get_value(1) . "\n";
echo get_value(2) . "\n";
echo get_value(3) . "\n";

?>
--EXPECTF--
1
2

Fatal error: Uncaught UnhandledMatchError: Unhandled match case 3 in %s:%d
Stack trace:
#0 %s: get_value(3)
#1 {main}
  thrown in %s on line %d
