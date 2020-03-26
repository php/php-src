--TEST--
Switch statement multiple conditions per case
--FILE--
<?php

function is_working_day($day) {
    switch ($day) {
        case 1, 7:
            return false;
        case 2, 3, 4, 5, 6:
            return true;
    };

    return null;
}

for ($i = 0; $i <= 8; $i++) {
    var_dump(is_working_day($i));
}

?>
--EXPECT--
NULL
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
NULL
