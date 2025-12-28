--TEST--
OSS-Fuzz #438780145: Nested finally with repeated return type check may uaf
--FILE--
<?php

function &test(): int {
    $x = 0;
    try {
        return $x;
    } finally {
        try {
            return $x;
        } finally {
            $x = "";
        }
    }
}

test();

?>
--EXPECTF--
Fatal error: Uncaught TypeError: test(): Return value must be of type int, string returned in %s:%d
Stack trace:
#0 %s(%d): test()
#1 {main}
  thrown in %s on line %d
