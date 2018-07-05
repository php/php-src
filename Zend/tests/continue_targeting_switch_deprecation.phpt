--TEST--
continue targeting switch is deprecated
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
Deprecated: Continues targeting switch are deprecated, use break instead in %s on line 6

Deprecated: Continues targeting switch are deprecated, use break instead in %s on line 14

Deprecated: Continues targeting switch are deprecated, use break instead in %s on line 26
