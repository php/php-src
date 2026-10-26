--TEST--
Test ignore_repeated_errors with interleaved error types
--INI--
ignore_repeated_errors=1
error_reporting=-1
--FILE--
<?php
for ($i = 0; $i < 3; $i++) {
    trim($missing);
}
?>
--EXPECTF--
Warning: Undefined variable $missing in %s on line %d

Deprecated: trim(): Passing null to parameter #1 ($string) of type string is deprecated in %s on line %d
