--TEST--
COM: General variant tests
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("com_dotnet")) print "skip COM/.Net support not present"; ?>
--FILE--
<?php // $Id$
error_reporting(E_ALL);

$v = new VARIANT();
if (VT_EMPTY != variant_get_type($v)) {
	echo "VT_EMPTY: bork\n";
}

$values = array(VT_I4 => 42, VT_R8 => 3.5, VT_BSTR => "hello", VT_BOOL => false);
$binary_ops = array('add', 'cat', 'sub', 'mul', 'and', 'div',
	'eqv', 'idiv', 'imp', 'mod', 'or', 'pow', 'xor');

foreach ($values as $t => $val) {
	$v = new VARIANT($val);
	if ($t != variant_get_type($v)) {
		printf("Bork: [%d] %d: %s\n", $t, variant_get_type($v), (string)$v);
	}
	$results = array();

	foreach ($values as $op2) {
		echo "--\n";
		foreach ($binary_ops as $op) {
			echo "$op: " . call_user_func('variant_' . $op, $v, $op2) . "\n";
		}
	}
}

echo "OK!";
?>
--EXPECT--
--
add: 84
cat: 4242
sub: 0
mul: 1764
and: 42
div: 1
eqv: -1
idiv: 1
imp: -1
mod: 0
or: 42
pow: 1.50130937545297E+68
xor: 0
--
add: 45.5
cat: 423.5
sub: 38.5
mul: 147
and: 0
div: 12
eqv: -47
idiv: 10
imp: -43
mod: 2
or: 46
pow: 480145.116863642
xor: 46
--
add: 
cat: 42hello
sub: 
mul: 
and: 
div: 
eqv: 
idiv: 
imp: 
mod: 
or: 
pow: 
xor: 
--
add: 42
cat: 42False
sub: 42
mul: 0
and: 0
div: 
eqv: -43
idiv: 
imp: -43
mod: 
or: 42
pow: 1
xor: 42
--
add: 45.5
cat: 3.542
sub: -38.5
mul: 147
and: 0
div: 8.33333333333333E-02
eqv: -47
idiv: 0
imp: -5
mod: 4
or: 46
pow: 7.09345573078604E+22
xor: 46
--
add: 7
cat: 3.53.5
sub: 0
mul: 12.25
and: 4
div: 1
eqv: -1
idiv: 1
imp: -1
mod: 0
or: 4
pow: 80.2117802289664
xor: 0
--
add: 
cat: 3.5hello
sub: 
mul: 
and: 
div: 
eqv: 
idiv: 
imp: 
mod: 
or: 
pow: 
xor: 
--
add: 3.5
cat: 3.5False
sub: 3.5
mul: 0
and: 0
div: 
eqv: -5
idiv: 
imp: -5
mod: 
or: 4
pow: 1
xor: 4
--
add: 
cat: hello42
sub: 
mul: 
and: 
div: 
eqv: 
idiv: 
imp: 
mod: 
or: 
pow: 
xor: 
--
add: 
cat: hello3.5
sub: 
mul: 
and: 
div: 
eqv: 
idiv: 
imp: 
mod: 
or: 
pow: 
xor: 
--
add: hellohello
cat: hellohello
sub: 
mul: 
and: 
div: 
eqv: 
idiv: 
imp: 
mod: 
or: 
pow: 
xor: 
--
add: 
cat: helloFalse
sub: 
mul: 
and: 
div: 
eqv: 
idiv: 
imp: 
mod: 
or: 
pow: 
xor: 
--
add: 42
cat: False42
sub: -42
mul: 0
and: 0
div: 0
eqv: -43
idiv: 0
imp: -1
mod: 0
or: 42
pow: 0
xor: 42
--
add: 3.5
cat: False3.5
sub: -3.5
mul: 0
and: 0
div: 0
eqv: -5
idiv: 0
imp: -1
mod: 0
or: 4
pow: 0
xor: 4
--
add: 
cat: Falsehello
sub: 
mul: 
and: 
div: 
eqv: 
idiv: 
imp: 
mod: 
or: 
pow: 
xor: 
--
add: 0
cat: FalseFalse
sub: 0
mul: 0
and: 0
div: 
eqv: -1
idiv: 
imp: -1
mod: 
or: 0
pow: 1
xor: 0
OK!
