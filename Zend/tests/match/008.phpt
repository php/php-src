--TEST--
Match expression multiple conditions per case
--FILE--
<?php

function is_working_day($day) {
    return match ($day) {
        1, 7 => false,
        2, 3, 4, 5, 6 => true,
    };
}

for ($i = 1; $i <= 7; $i++) {
    var_dump(is_working_day($i));
}

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
