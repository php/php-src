--TEST--
#[\NoDiscard]: positional and named message, null, and the errors of the constructor
--FILE--
<?php

#[\NoDiscard("positional")]
function positional_message() { return 1; }
#[\NoDiscard(message: "named")]
function named_message() { return 1; }
#[\NoDiscard(null)]
function null_message() { return 1; }
#[\NoDiscard("")]
function empty_message() { return 1; }
#[\NoDiscard(since: "1.0")]
function since_named() { return 1; }
#[\NoDiscard("a", "b")]
function too_many() { return 1; }
#[\NoDiscard(unknown: "x")]
function unknown_named() { return 1; }

foreach (['positional_message', 'named_message', 'null_message', 'empty_message', 'since_named', 'too_many', 'unknown_named'] as $f) {
    try {
        $f();
    } catch (Throwable $e) {
        echo get_class($e), ": ", $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--

Warning: The return value of function positional_message() should either be used or intentionally ignored by casting it as (void), positional in %s on line %d

Warning: The return value of function named_message() should either be used or intentionally ignored by casting it as (void), named in %s on line %d

Warning: The return value of function null_message() should either be used or intentionally ignored by casting it as (void) in %s on line %d

Warning: The return value of function empty_message() should either be used or intentionally ignored by casting it as (void) in %s on line %d
Error: Unknown named parameter $since
ArgumentCountError: NoDiscard::__construct() expects at most 1 argument, 2 given
Error: Unknown named parameter $unknown
