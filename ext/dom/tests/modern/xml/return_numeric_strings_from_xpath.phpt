--TEST--
Returning numeric string values from Dom\XPath callback functions for large integers
--EXTENSIONS--
dom
--SKIPIF--
<?php if (PHP_INT_SIZE !== 8) die("skip 64-bit only"); ?>
--FILE--
<?php
define('MAX_SAFE_INTEGER', 2**53-1);

$document = Dom\XMLDocument::createFromString('<root/>');
$xpath = new Dom\XPath($document);
$xpath->registerPhpFunctionNs('urn:x', 'get-int',                   fn(int $i): int => $i);
$xpath->registerPhpFunctionNs('urn:x', 'get-float',                 fn(float $f): float => $f);
$xpath->registerPhpFunctionNs('urn:x', 'get-max-safe-int',          fn() =>  MAX_SAFE_INTEGER);
$xpath->registerPhpFunctionNs('urn:x', 'get-negative-max-safe-int', fn() => -MAX_SAFE_INTEGER);
$xpath->registerPhpFunctionNs('urn:x', 'get-large-int',             fn() =>  MAX_SAFE_INTEGER+2);
$xpath->registerPhpFunctionNs('urn:x', 'get-negative-large-int',    fn() => -MAX_SAFE_INTEGER-2);
$xpath->registerNamespace('x', 'urn:x');

var_dump($xpath->evaluate('x:get-float('.PHP_INT_MAX.')'));
var_dump($xpath->evaluate('x:get-float('.PHP_INT_MIN.')'));
var_dump($xpath->evaluate('x:get-float('.PHP_FLOAT_MAX.')'));
var_dump($xpath->evaluate('x:get-float('.PHP_FLOAT_MIN.')'));
var_dump($xpath->evaluate('x:get-float('.PHP_FLOAT_EPSILON.')'));

var_dump(MAX_SAFE_INTEGER);
var_dump(floatval(MAX_SAFE_INTEGER));
var_dump($xpath->evaluate("x:get-int(".(MAX_SAFE_INTEGER).")"));
var_dump($xpath->evaluate("x:get-int(".(-MAX_SAFE_INTEGER).")"));
var_dump($xpath->evaluate("x:get-max-safe-int()"));
var_dump($xpath->evaluate("x:get-negative-max-safe-int()"));

var_dump(MAX_SAFE_INTEGER+2);
// loses precision while type casting
var_dump(floatval(MAX_SAFE_INTEGER+2));
// loses precision while parameter passing int -> XPath number
var_dump($xpath->evaluate("x:get-int(".(MAX_SAFE_INTEGER+2).")"));
var_dump($xpath->evaluate("x:get-int(".(-MAX_SAFE_INTEGER-2).")"));
// returns string values for integers larger than 2^53-1 to maintain precision
var_dump($xpath->evaluate("x:get-large-int()"));
var_dump($xpath->evaluate("x:get-negative-large-int()"));
?>
--EXPECT--
float(9.223372036854778E+18)
float(-9.223372036854778E+18)
float(1.7976931348623E+308)
float(2.2250738585072E-308)
float(2.2204460492503003E-16)
int(9007199254740991)
float(9007199254740991)
float(9007199254740991)
float(-9007199254740991)
float(9007199254740991)
float(-9007199254740991)
int(9007199254740993)
float(9007199254740992)
string(16) "9007199254740992"
string(17) "-9007199254740992"
string(16) "9007199254740993"
string(17) "-9007199254740993"
