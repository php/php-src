--TEST--
putenv() rejects null bytes
--FILE--
<?php

$var_name = 'PHP_PUTENV_NUL_TEST';

foreach ([
    $var_name . "\0SUFFIX=value",
    $var_name . "=va\0lue",
] as $assignment) {
    try {
        putenv($assignment);
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }
}

var_dump(getenv($var_name));

?>
--EXPECT--
putenv(): Argument #1 ($assignment) must not contain any null bytes
putenv(): Argument #1 ($assignment) must not contain any null bytes
bool(false)
