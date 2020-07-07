--TEST--
setcookie() error tests
--INI--
date.timezone=UTC
--FILE--
<?php

ob_start();

try {
    setcookie('');
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    setcookie('invalid=');
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    setcookie('name', 'invalid;');
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    setcookie('name', 'value', 100, 'invalid;');
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    setcookie('name', 'value', 100, 'path', 'invalid;');
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

var_dump(headers_list());
--EXPECTHEADERS--

--EXPECTF--
setcookie(): Argument #1 ($name) cannot be empty
setcookie(): Argument #1 ($name) cannot contain "=", ",", ";", " ", "\t", "\r", "\n", "\013", and "\014"
setcookie(): Argument #4 ($path) cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", and "\014"
setcookie(): Argument #5 ($domain) cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", and "\014"
array(2) {
  [0]=>
  string(%d) "X-Powered-By: PHP/%s"
  [1]=>
  string(27) "Set-Cookie: name=invalid%3B"
}
