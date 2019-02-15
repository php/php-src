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
setcookie('name2', 'value2', [0 => 'numeric_key']);
// Unrecognized key
setcookie('name3', 'value3', ['path' => '/path/', 'foo' => 'bar']);
// Arguments after options array (will not be set)
setcookie('name4', 'value4', [], "path", "domain.tld", true, true);

var_dump(headers_list());
--EXPECTHEADERS--

--EXPECTF--
Warning: setcookie(): Unrecognized key 'unknown_key' found in the options array in %s

Warning: setcookie(): No valid options were found in the given array in %s

Warning: setcookie(): Numeric key found in the options array in %s

Warning: setcookie(): No valid options were found in the given array in %s

Warning: setcookie(): Unrecognized key 'foo' found in the options array in %s

Warning: setcookie(): Cannot pass arguments after the options array in %s
array(4) {
  [0]=>
  string(%d) "X-Powered-By: PHP/%s"
  [1]=>
  string(22) "Set-Cookie: name=value"
  [2]=>
  string(24) "Set-Cookie: name2=value2"
  [3]=>
  string(37) "Set-Cookie: name3=value3; path=/path/"
}
