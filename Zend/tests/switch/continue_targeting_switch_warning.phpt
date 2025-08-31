--TEST--
Warning on continue targeting switch
--FILE--
<?php

function test() {
    switch ($foo) {
        case 0:
            continue; // INVALID
        case 1:
            break;
    }

    while ($foo) {
        switch ($bar) {
            case 0:
                continue; // INVALID
            case 1:
                continue 2;
            case 2:
                break;
        }
    }

    switch ($bar) {
        case 0:
            while ($xyz) {
                continue 2; // INVALID
            }
        case 1:
            while ($xyz) {
                continue;
            }
        case 2:
            while ($xyz) {
                break 2;
            }
    }

    while ($foo) {
        switch ($bar) {
            case 0:
                while ($xyz) {
                    continue 2; // INVALID
                }
            case 1:
                while ($xyz) {
                    continue 3;
                }
            case 2:
                while ($xyz) {
                    break 2;
                }
        }
    }
}

?>
--EXPECTF--
Warning: "continue" targeting switch is equivalent to "break" in %s on line 6

Warning: "continue" targeting switch is equivalent to "break". Did you mean to use "continue 2"? in %s on line 14

Warning: "continue 2" targeting switch is equivalent to "break 2" in %s on line 25

Warning: "continue 2" targeting switch is equivalent to "break 2". Did you mean to use "continue 3"? in %s on line 41
