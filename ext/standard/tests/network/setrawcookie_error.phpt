--TEST--
setrawcookie() error tests
--INI--
date.timezone=UTC
--FILE--
<?php

ob_start();

try {
    setrawcookie('');
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    setrawcookie('invalid=');
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    setrawcookie('name', 'invalid;');
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
// To go above year 9999: 60 * 60 * 24 * 365 * 9999
try {
    setrawcookie('name', 'value', 315328464000);
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    setrawcookie('name', 'value', 100, 'invalid;');
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    setrawcookie('name', 'value', 100, 'path', 'invalid;');
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

var_dump(headers_list());
--EXPECTHEADERS--

--EXPECTF--
setrawcookie(): Argument #1 ($name) cannot be empty
setrawcookie(): Argument #1 ($name) cannot contain "=", ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
setrawcookie(): Argument #2 ($value) cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
setrawcookie(): "expires" option cannot have a year greater than 9999
setrawcookie(): "path" option cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
setrawcookie(): "domain" option cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
array(1) {
  [0]=>
  string(%d) "X-Powered-By: PHP/%s"
}
