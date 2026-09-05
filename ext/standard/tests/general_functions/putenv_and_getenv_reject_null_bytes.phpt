--TEST--
getenv() and putenv() reject null bytes
--FILE--
<?php

foreach ([false, true] as $local_only) {
    try {
        getenv("PHP_GETENV_NUL_TEST\0SUFFIX", $local_only);
    } catch (Throwable $exception) {
        echo $exception::class, ': ', $exception->getMessage(), "\n";
    }
}

$var_name = 'PHP_PUTENV_NUL_TEST';

foreach ([
    $var_name . "\0SUFFIX=value",
    $var_name . "=va\0lue",
] as $assignment) {
    try {
        putenv($assignment);
    } catch (Throwable $exception) {
        echo $exception::class, ': ', $exception->getMessage(), "\n";
    }
}

var_dump(getenv($var_name));

?>
--EXPECT--
ValueError: getenv(): Argument #1 ($name) must not contain any null bytes
ValueError: getenv(): Argument #1 ($name) must not contain any null bytes
ValueError: putenv(): Argument #1 ($assignment) must not contain any null bytes
ValueError: putenv(): Argument #1 ($assignment) must not contain any null bytes
bool(false)
