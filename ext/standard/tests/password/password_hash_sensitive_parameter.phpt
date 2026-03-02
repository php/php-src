--TEST--
Test that the password parameter is marked sensitive.
--FILE--
<?php
try {
    var_dump(password_hash("foo"));
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
try {
    var_dump(password_hash("foo", "Invalid"));
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
?>
--EXPECTF--
ArgumentCountError: password_hash() expects at least 2 arguments, 1 given in %spassword_hash_sensitive_parameter.php:3
Stack trace:
#0 %spassword_hash_sensitive_parameter.php(3): password_hash(Object(SensitiveParameterValue))
#1 {main}
ValueError: password_hash(): Argument #2 ($algo) must be a valid password hashing algorithm in%spassword_hash_sensitive_parameter.php:8
Stack trace:
#0 %spassword_hash_sensitive_parameter.php(8): password_hash(Object(SensitiveParameterValue), 'Invalid')
#1 {main}
