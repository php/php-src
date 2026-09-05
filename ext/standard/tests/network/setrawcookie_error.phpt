--TEST--
setrawcookie() error tests
--INI--
date.timezone=UTC
--FILE--
<?php

ob_start();

try {
    setrawcookie('');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    setrawcookie('invalid=');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    setrawcookie('name', 'invalid;');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    setrawcookie('name', 'value', 100, 'invalid;');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    setrawcookie('name', 'value', 100, 'path', 'invalid;');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

if (PHP_INT_SIZE == 8) {
    try {
        // To go above year 9999: 60 * 60 * 24 * 365 * 9999
        setrawcookie('name', 'value', 315328464000);
    } catch (\ValueError $e) {
        var_dump($e->getMessage() == 'setrawcookie(): "expires" option cannot have a year greater than 9999');
    }
} else {
    var_dump(true);
}

var_dump(headers_list());

?>
--EXPECTHEADERS--

--EXPECTF--
ValueError: setrawcookie(): Argument #1 ($name) must not be empty
ValueError: setrawcookie(): Argument #1 ($name) cannot contain "=", ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
ValueError: setrawcookie(): Argument #2 ($value) cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
ValueError: setrawcookie(): "path" option cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
ValueError: setrawcookie(): "domain" option cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
bool(true)
array(1) {
  [0]=>
  string(%d) "X-Powered-By: PHP/%s"
}
