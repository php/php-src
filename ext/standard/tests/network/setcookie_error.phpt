--TEST--
setcookie() error tests
--INI--
date.timezone=UTC
--FILE--
<?php

ob_start();

try {
    setcookie('');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    setcookie('invalid=');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    setcookie('name', 'invalid;');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    setcookie('name', 'value', 100, 'invalid;');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    setcookie('name', 'value', 100, 'path', 'invalid;');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

if (PHP_INT_SIZE == 8) {
    try {
        // To go above year 9999: 60 * 60 * 24 * 365 * 9999
        setcookie('name', 'value', 315328464000);
    } catch (\ValueError $e) {
        var_dump($e->getMessage() == 'setcookie(): "expires" option cannot have a year greater than 9999');
    }
} else {
    var_dump(true);
}

var_dump(headers_list());

?>
--EXPECTHEADERS--

--EXPECTF--
ValueError: setcookie(): Argument #1 ($name) must not be empty
ValueError: setcookie(): Argument #1 ($name) cannot contain "=", ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
ValueError: setcookie(): "path" option cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
ValueError: setcookie(): "domain" option cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
bool(true)
array(2) {
  [0]=>
  string(%d) "X-Powered-By: PHP/%s"
  [1]=>
  string(27) "Set-Cookie: name=invalid%3B"
}
