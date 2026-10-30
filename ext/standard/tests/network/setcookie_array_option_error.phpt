--TEST--
setcookie() array variant error tests
--INI--
date.timezone=UTC
--FILE--
<?php

ob_start();

// Unrecognized key and no valid keys
try {
    setcookie('name', 'value', ['unknown_key' => 'only']);
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
// Numeric key and no valid keys
try {
    setcookie('name2', 'value2', [0 => 'numeric_key']);
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
// Unrecognized key
try {
    setcookie('name3', 'value3', ['path' => '/path/', 'foo' => 'bar']);
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
// Invalid path key content
try {
    setcookie('name', 'value', ['path' => '/;/']);
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
// Invalid domain key content
try {
    setcookie('name', 'value', ['path' => '/path/', 'domain' => 'ba;r']);
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

// Arguments after options array (will not be set)
try {
    setcookie('name4', 'value4', [], "path", "domain.tld", true, true);
} catch (\ArgumentCountError $e) {
    echo $e->getMessage() . "\n";
}

if (PHP_INT_SIZE == 8) {
    try {
        // To go above year 9999: 60 * 60 * 24 * 365 * 9999
        setcookie('name', 'value', ['expires' => 315328464000]);
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
setcookie(): option "unknown_key" is invalid
setcookie(): option array cannot have numeric keys
setcookie(): option "foo" is invalid
setcookie(): "path" option cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
setcookie(): "domain" option cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
setcookie(): Expects exactly 3 arguments when argument #3 ($expires_or_options) is an array
bool(true)
array(1) {
  [0]=>
  string(%s) "X-Powered-By: PHP/%s"
}
