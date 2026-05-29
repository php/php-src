--TEST--
Returning scalar values from Dom\XPath callback functions
--EXTENSIONS--
dom
--FILE--
<?php
$document = Dom\XMLDocument::createFromString('<root/>');
$xpath = new Dom\XPath($document);
$xpath->registerPhpFunctionNs('urn:x', 'get-int',    fn(int $i): int => $i);
$xpath->registerPhpFunctionNs('urn:x', 'get-float',  fn(float $f): float => $f);
$xpath->registerPhpFunctionNs('urn:x', 'get-string', fn(string $s): string => $s);
$xpath->registerPhpFunctionNs('urn:x', 'get-bool',   fn(bool $b): bool => $b);
$xpath->registerNamespace('x', 'urn:x');

var_dump($xpath->evaluate('x:get-string("test")'));
var_dump($xpath->evaluate('x:get-bool(1)'));
var_dump($xpath->evaluate('x:get-bool(0)'));

var_dump($xpath->evaluate('x:get-int(41)'));
var_dump($xpath->evaluate('x:get-int(41) + 1'));
var_dump($xpath->evaluate('x:get-int(41)') + 1);
var_dump($xpath->evaluate('x:get-float(4.2)'));
var_dump($xpath->evaluate('2 * x:get-float(4.2)'));
var_dump(2 * $xpath->evaluate('x:get-float(4.2)'));

var_dump($xpath->evaluate('x:get-int(0)'));
var_dump($xpath->evaluate('x:get-int(-0)'));
var_dump($xpath->evaluate('x:get-float(0)'));
var_dump($xpath->evaluate('x:get-float(-0)'));

var_dump($xpath->evaluate('x:get-float(number("invalid"))'));
var_dump($xpath->evaluate('x:get-float(1 div 0)'));
?>
--EXPECT--
string(4) "test"
bool(true)
bool(false)
float(41)
float(42)
float(42)
float(4.2)
float(8.4)
float(8.4)
float(0)
float(0)
float(0)
float(-0)
float(NAN)
float(INF)
