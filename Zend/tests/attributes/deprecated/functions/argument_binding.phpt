--TEST--
#[\Deprecated]: positional and named arguments, null, and the errors of the constructor
--FILE--
<?php

#[\Deprecated("message only")]
function positional_message() {}
#[\Deprecated(since: "1.0")]
function named_since() {}
#[\Deprecated("m", "1.0")]
function both_positional() {}
#[\Deprecated(since: "1.0", message: "m")]
function named_reversed() {}
#[\Deprecated(null, since: "1.0")]
function null_message() {}
#[\Deprecated("m", null)]
function null_since() {}
#[\Deprecated("", "")]
function empty_strings() {}
#[\Deprecated("a", message: "b")]
function positional_then_same_named() {}
#[\Deprecated(null, message: "b")]
function null_then_same_named() {}
#[\Deprecated(unknown: "x")]
function unknown_named() {}
#[\Deprecated(Message: "x")]
function wrong_case() {}
#[\Deprecated("a", "b", "c")]
function too_many() {}

foreach ([
    'positional_message', 'named_since', 'both_positional', 'named_reversed', 'null_message',
    'null_since', 'empty_strings', 'positional_then_same_named', 'null_then_same_named',
    'unknown_named', 'wrong_case', 'too_many',
] as $f) {
    try {
        $f();
    } catch (Throwable $e) {
        echo get_class($e), ": ", $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--

Deprecated: Function positional_message() is deprecated, message only in %s on line %d

Deprecated: Function named_since() is deprecated since 1.0 in %s on line %d

Deprecated: Function both_positional() is deprecated since 1.0, m in %s on line %d

Deprecated: Function named_reversed() is deprecated since 1.0, m in %s on line %d

Deprecated: Function null_message() is deprecated since 1.0 in %s on line %d

Deprecated: Function null_since() is deprecated, m in %s on line %d

Deprecated: Function empty_strings() is deprecated in %s on line %d
Error: Named parameter $message overwrites previous argument
Error: Named parameter $message overwrites previous argument
Error: Unknown named parameter $unknown
Error: Unknown named parameter $Message
ArgumentCountError: Deprecated::__construct() expects at most 2 arguments, 3 given
