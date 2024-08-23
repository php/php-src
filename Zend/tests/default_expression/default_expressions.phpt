--TEST--
Tests an exhaustive list of valid expressions containing the default keyword
--FILE--
<?php

function F($V = 2) { return $V; }

echo "Numeric binary operators (LHS)\n";
var_dump(F(default + 1));
var_dump(F(default - 1));
var_dump(F(default * 2));
var_dump(F(default / 2));
var_dump(F(default % 2));
var_dump(F(default & 1));
var_dump(F(default | 1));
var_dump(F(default ^ 2));
var_dump(F(default << 1));
var_dump(F(default >> 1));
var_dump(F(default ** 2));
var_dump(F(default <=> 2));

echo "\nNumeric binary operators (RHS)\n";
var_dump(F(1 + default));
var_dump(F(1 - default));
var_dump(F(2 * default));
var_dump(F(2 / default));
var_dump(F(2 % default));
var_dump(F(1 & default));
var_dump(F(1 | default));
var_dump(F(2 ^ default));
var_dump(F(1 << default));
var_dump(F(1 >> default));
var_dump(F(2 ** default));
var_dump(F(2 <=> default));

echo "\nBoolean binary operators\n";
var_dump(F(default === 2));
var_dump(F(default !== 2));
var_dump(F(default == '2'));
var_dump(F(default != '2'));
var_dump(F(default >= 1));
var_dump(F(default <= 1));
var_dump(F(default > 1));
var_dump(F(default < 1));
var_dump(F(default && 0));
var_dump(F(default || 0));
var_dump(F(default and 0));
var_dump(F(default or 0));
var_dump(F(default xor 0));

echo "\nUnary operators\n";
var_dump(F(+default));
var_dump(F(-default));
var_dump(F(!default));
var_dump(F(~default));

echo "\nConditional expressions\n";
var_dump(F(default ? 1 : 0));
var_dump(F(1 ? default : 0));
var_dump(F(1 ? 1 : default));
var_dump(F(default ?: 0));
var_dump(F(0 ?: default));
var_dump(F(default ?? 0));
var_dump(F(null ?? default));

echo "\nCasts\n";
var_dump(F((int)default));
var_dump(F((double)default));
var_dump(F((string)default));
var_dump(F((array)default));
var_dump(F((object)default));
var_dump(F((bool)default));

echo "\nInternal functions\n";
var_dump(F(empty(default)));
// eval() makes no sense.
// exit() makes no sense.
// TODO?
// include()
// include_once()
// require()
// require_once()

echo "\nMatch tier\n";
var_dump(F(match(default) { default => default }));
var_dump(F(match(default) { 2 => 3 }));

echo "\nPrint tier\n";
var_dump(F(print default));
?>
--EXPECT--
Numeric binary operators (LHS)
int(3)
int(1)
int(4)
int(1)
int(0)
int(0)
int(3)
int(0)
int(4)
int(1)
int(4)
int(0)

Numeric binary operators (RHS)
int(3)
int(-1)
int(4)
int(1)
int(0)
int(0)
int(3)
int(0)
int(4)
int(0)
int(4)
int(0)

Boolean binary operators
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)

Unary operators
int(2)
int(-2)
bool(false)
int(-3)

Conditional expressions
int(1)
int(2)
int(1)
int(2)
int(2)
int(2)
int(2)

Casts
int(2)
float(2)
string(1) "2"
array(1) {
  [0]=>
  int(2)
}
object(stdClass)#1 (1) {
  ["scalar"]=>
  int(2)
}
bool(true)

Internal functions
bool(false)

Match tier
int(2)
int(3)

Print tier
2int(1)
