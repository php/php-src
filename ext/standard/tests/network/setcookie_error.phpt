--TEST--
setcookie() array variant error tests
--INI--
date.timezone=UTC
--FILE--
<?php

ob_start();

// Unrecognized key and no valid keys
setcookie('name', 'value', ['unknown_key' => 'only']);
// Numeric key and no valid keys
setcookie('name', 'value', [0 => 'numeric_key']);
// Unrecognized key
setcookie('name', 'value', ['path' => '/path/', 'foo' => 'bar']);

--EXPECTF--
Warning: setcookie(): Unrecognized key 'unknown_key' found in the options array in %s

Warning: setcookie(): No valid options were found in the given array in %s

Warning: setcookie(): Numeric key found in the options array in %s

Warning: setcookie(): No valid options were found in the given array in %s

Warning: setcookie(): Unrecognized key 'foo' found in the options array in %s
