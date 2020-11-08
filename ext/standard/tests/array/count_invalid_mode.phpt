--TEST--
Test count() function : invalid modes in weak type mode
--FILE--
<?php

$modes = [
    COUNT_NORMAL,
    COUNT_RECURSIVE,
    0,
    1,
    -1,
    -1.45,
    2,
    TRUE,
    FALSE,
    NULL,
];

foreach ($modes as $mode) {
    try {
        var_dump(count([], $mode));
    } catch (\ValueError $error) {
        echo $error->getMessage() . \PHP_EOL;
    }
}
?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
count(): Argument #2 ($mode) must be either COUNT_NORMAL or COUNT_RECURSIVE
count(): Argument #2 ($mode) must be either COUNT_NORMAL or COUNT_RECURSIVE
count(): Argument #2 ($mode) must be either COUNT_NORMAL or COUNT_RECURSIVE
int(0)
int(0)
int(0)
