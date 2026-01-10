--TEST--
Test session_create_id() function : invalid prefix
--INI--
session.save_handler=files
session.sid_length=32
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

var_dump(session_create_id('_'));
var_dump(session_create_id('%'));

try {
    var_dump(session_create_id('ABTgdPs68S3M4HMaqKwj33TzqLMv5PHpWQxJbfpeogEhrJRY7o9f33pKLCmhf0tXCtoBkIu0yxXYCSHfJhPd2miPUW4MIpd91dnEiOwWDfaBnfdJZOwgvgmYLSfDGaebqmnCAoyuzlcq2j59nNRhccgJIkr9ytY3RwFTTXszpcjpx6mlJuG9GksKAhPsnnaEwSEb0eFyqvn80gYI2roKSjaFSmJxg0xgXuCF4csMo8DxiSvovho5QTKx5u7h8VyQL'));
} catch (Throwable $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

try {
    var_dump(session_create_id("AB\0CD"));
} catch (Throwable $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

?>
Done
--EXPECTF--
Warning: session_create_id(): Prefix cannot contain special characters. Only the A-Z, a-z, 0-9, "-", and "," characters are allowed in %s on line %d
bool(false)

Warning: session_create_id(): Prefix cannot contain special characters. Only the A-Z, a-z, 0-9, "-", and "," characters are allowed in %s on line %d
bool(false)
ValueError: session_create_id(): Argument #1 ($prefix) cannot be longer than 256 characters
ValueError: session_create_id(): Argument #1 ($prefix) must not contain any null bytes
Done
