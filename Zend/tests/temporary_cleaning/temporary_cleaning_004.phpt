--TEST--
Temporary leak with switch
--FILE--
<?php

function ops() {
    throw new Exception();
}

$a = [new stdClass, new stdClass];
switch ($a[0]) {
    case false:
    break;
    default:
        try {
            $x = 2;
            $y = new stdClass;
            while ($x-- && new stdClass) {
                $r = [$x] + ($y ? ((array) $x) + [2] : ops());
                $y = (array) $y;
            }
        } catch (Exception $e) {
        }
}

try {
    switch ($a[0]) {
        case false:
        break;
        default:
            $x = 2;
            $y = new stdClass;
            while ($x-- && new stdClass) {
                $r = [$x] + ($y ? ((array) $x) + [2] : ops());
                $y = (array) $y;
            }
    }
} catch (Exception $e) {
}

?>
==DONE==
--EXPECT--
==DONE==
