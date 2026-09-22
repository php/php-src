--TEST--
GHSA-74r9-qxhc-fx53: Out-of-bounds access in mbfl_name2encoding_ex()
--CREDITS--
Akshay Jain (AkshayJainG)
--EXTENSIONS--
mbstring
--FILE--
<?php

$encoding = "UTF-8\x00AAAAAAAAAAAAAAAA";
$alias = "binary\x00AAAAAAAAAAAAAAAA";
$var = 'foo';

function test($c) {
    try {
        $c();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

ini_set('mbstring.detect_order', $encoding);
ini_set('mbstring.detect_order', $alias);
ini_set('mbstring.http_output', $encoding);
ini_set('mbstring.http_output', $alias);

test(fn () => mb_convert_encoding('foo', $encoding, $encoding));
test(fn () => mb_convert_encoding('foo', $alias, $alias));
test(fn () => mb_detect_encoding('foo', $encoding));
test(fn () => mb_detect_encoding('foo', $alias));
test(fn () => mb_convert_variables($encoding, $alias, $var));
test(fn () => mb_detect_order($encoding));
test(fn () => mb_detect_order($alias));

?>
--EXPECTF--
Warning: ini_set(): INI setting contains invalid encoding "UTF-8" in %s on line %d

Warning: ini_set(): INI setting contains invalid encoding "binary" in %s on line %d

Deprecated: ini_set(): Use of mbstring.http_output is deprecated in %s on line %d

Deprecated: ini_set(): Use of mbstring.http_output is deprecated in %s on line %d
ValueError: mb_convert_encoding(): Argument #3 ($from_encoding) contains invalid encoding "UTF-8"
ValueError: mb_convert_encoding(): Argument #3 ($from_encoding) contains invalid encoding "binary"
ValueError: mb_detect_encoding(): Argument #2 ($encodings) contains invalid encoding "UTF-8"
ValueError: mb_detect_encoding(): Argument #2 ($encodings) contains invalid encoding "binary"
ValueError: mb_convert_variables(): Argument #2 ($from_encoding) contains invalid encoding "binary"
ValueError: mb_detect_order(): Argument #1 ($encoding) contains invalid encoding "UTF-8"
ValueError: mb_detect_order(): Argument #1 ($encoding) contains invalid encoding "binary"
