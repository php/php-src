--TEST--
Warning on switch case falling through
--FILE--
<?php

function test($foo) {
    switch ($foo) {
        case 0:
        case 1:
            echo "a", PHP_EOL;
            break;
        case 2:
            echo "b", PHP_EOL;
        case 3:
            echo "c", PHP_EOL;
            break;
        case 4:
            echo "d", PHP_EOL;
            fallthrough;
        case 5:
            echo "e", PHP_EOL;
            break;
        case 6:
            echo "f", PHP_EOL;
            return;
        case 7:
            echo "g", PHP_EOL;
            continue;
        case 8:
            echo "h", PHP_EOL;
            goto end;
        case 9:
            echo "i", PHP_EOL;
            throw new \Exception("exception");
    }
    end:
}

for ($i = 0; $i <= 9; $i++) {
    try {
        test($i);
    } catch (\Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECTF--
Warning: Non-empty case falls through to the next case, terminate the case with "fallthrough;" if this is intentional in %s on line 9

Warning: "continue" targeting switch is equivalent to "break" in %s on line 25
a
a
b
c
c
d
e
e
f
g
h
i
Exception: exception
