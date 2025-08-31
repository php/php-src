--TEST--
Test that sensitive parameters are marked sensitive.
--FILE--
<?php
try {
    var_dump(hash_equals('foo', null));
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
try {
    var_dump(hash_hmac('foo', 'bar', 'baz'));
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
?>
--EXPECTF--
TypeError: hash_equals(): Argument #2 ($user_string) must be of type string, null given in %s:%d
Stack trace:
#0 %s(%d): hash_equals(Object(SensitiveParameterValue), Object(SensitiveParameterValue))
#1 {main}
ValueError: hash_hmac(): Argument #1 ($algo) must be a valid cryptographic hashing algorithm in %s:%d
Stack trace:
#0 %s(%d): hash_hmac('foo', 'bar', Object(SensitiveParameterValue))
#1 {main}
