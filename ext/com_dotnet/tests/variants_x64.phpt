--TEST--
COM: General variant tests
--EXTENSIONS--
com_dotnet
--SKIPIF--
<?php
if (8 != PHP_INT_SIZE) print "skip x64 only";
if ((string) variant_cat(new VARIANT(false), new VARIANT(0.5)) != 'False0.5')
    print "skip English locale only";
?>
--FILE--
<?php
error_reporting(E_ALL);

$v = new VARIANT();
if (VT_EMPTY != variant_get_type($v)) {
    echo "VT_EMPTY: bork\n";
}

$values = array(VT_I8 => 42, VT_R8 => 3.5, VT_BSTR => "hello", VT_BOOL => false);
$binary_ops = array('add', 'cat', 'sub', 'mul', 'and', 'div',
    'eqv', 'idiv', 'imp', 'mod', 'or', 'pow', 'xor');

foreach ($values as $t => $val) {
    $v = new VARIANT($val);
    if ($t != variant_get_type($v)) {
        printf("Bork: [%d] %d: %s\n", $t, variant_get_type($v), $val);
        print $v . "\n";
    }
    $results = array();

    foreach ($values as $op2) {
        echo "--\n";
        foreach ($binary_ops as $op) {
            try {
                echo "$op: " . call_user_func('variant_' . $op, $v, $op2) . "\n";
            } catch (com_exception $e) {
                echo "$op:\n";
                echo "\tvariant_$op($v, $op2)\n";
                echo "\texception " . $e->getMessage();
                printf("\n\tcode %08x\n\n", $e->getCode());
            }
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
	variant_add(42, hello)
	exception Type mismatch
	code 80020005

cat: 42hello
sub:
	variant_sub(42, hello)
	exception Type mismatch
	code 80020005

mul:
	variant_mul(42, hello)
	exception Type mismatch
	code 80020005

and:
	variant_and(42, hello)
	exception Type mismatch
	code 80020005

div:
	variant_div(42, hello)
	exception Type mismatch
	code 80020005

eqv:
	variant_eqv(42, hello)
	exception Type mismatch
	code 80020005

idiv:
	variant_idiv(42, hello)
	exception Type mismatch
	code 80020005

imp:
	variant_imp(42, hello)
	exception Type mismatch
	code 80020005

mod:
	variant_mod(42, hello)
	exception Type mismatch
	code 80020005

or:
	variant_or(42, hello)
	exception Type mismatch
	code 80020005

pow:
	variant_pow(42, hello)
	exception Type mismatch
	code 80020005

xor:
	variant_xor(42, hello)
	exception Type mismatch
	code 80020005

--
add: 42
cat: 42False
sub: 42
mul: 0
and: 0
div:
	variant_div(42, )
	exception Division by zero
	code 80020012

eqv: -43
idiv:
	variant_idiv(42, )
	exception Division by zero
	code 80020012

imp: -43
mod:
	variant_mod(42, )
	exception Division by zero
	code 80020012

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
	variant_add(3.5, hello)
	exception Type mismatch
	code 80020005

cat: 3.5hello
sub:
	variant_sub(3.5, hello)
	exception Type mismatch
	code 80020005

mul:
	variant_mul(3.5, hello)
	exception Type mismatch
	code 80020005

and:
	variant_and(3.5, hello)
	exception Type mismatch
	code 80020005

div:
	variant_div(3.5, hello)
	exception Type mismatch
	code 80020005

eqv:
	variant_eqv(3.5, hello)
	exception Type mismatch
	code 80020005

idiv:
	variant_idiv(3.5, hello)
	exception Type mismatch
	code 80020005

imp:
	variant_imp(3.5, hello)
	exception Type mismatch
	code 80020005

mod:
	variant_mod(3.5, hello)
	exception Type mismatch
	code 80020005

or:
	variant_or(3.5, hello)
	exception Type mismatch
	code 80020005

pow:
	variant_pow(3.5, hello)
	exception Type mismatch
	code 80020005

xor:
	variant_xor(3.5, hello)
	exception Type mismatch
	code 80020005

--
add: 3.5
cat: 3.5False
sub: 3.5
mul: 0
and: 0
div:
	variant_div(3.5, )
	exception Division by zero
	code 80020012

eqv: -5
idiv:
	variant_idiv(3.5, )
	exception Division by zero
	code 80020012

imp: -5
mod:
	variant_mod(3.5, )
	exception Division by zero
	code 80020012

or: 4
pow: 1
xor: 4
--
add:
	variant_add(hello, 42)
	exception Type mismatch
	code 80020005

cat: hello42
sub:
	variant_sub(hello, 42)
	exception Type mismatch
	code 80020005

mul:
	variant_mul(hello, 42)
	exception Type mismatch
	code 80020005

and:
	variant_and(hello, 42)
	exception Type mismatch
	code 80020005

div:
	variant_div(hello, 42)
	exception Type mismatch
	code 80020005

eqv:
	variant_eqv(hello, 42)
	exception Type mismatch
	code 80020005

idiv:
	variant_idiv(hello, 42)
	exception Type mismatch
	code 80020005

imp:
	variant_imp(hello, 42)
	exception Type mismatch
	code 80020005

mod:
	variant_mod(hello, 42)
	exception Type mismatch
	code 80020005

or:
	variant_or(hello, 42)
	exception Type mismatch
	code 80020005

pow:
	variant_pow(hello, 42)
	exception Type mismatch
	code 80020005

xor:
	variant_xor(hello, 42)
	exception Type mismatch
	code 80020005

--
add:
	variant_add(hello, 3.5)
	exception Type mismatch
	code 80020005

cat: hello3.5
sub:
	variant_sub(hello, 3.5)
	exception Type mismatch
	code 80020005

mul:
	variant_mul(hello, 3.5)
	exception Type mismatch
	code 80020005

and:
	variant_and(hello, 3.5)
	exception Type mismatch
	code 80020005

div:
	variant_div(hello, 3.5)
	exception Type mismatch
	code 80020005

eqv:
	variant_eqv(hello, 3.5)
	exception Type mismatch
	code 80020005

idiv:
	variant_idiv(hello, 3.5)
	exception Type mismatch
	code 80020005

imp:
	variant_imp(hello, 3.5)
	exception Type mismatch
	code 80020005

mod:
	variant_mod(hello, 3.5)
	exception Type mismatch
	code 80020005

or:
	variant_or(hello, 3.5)
	exception Type mismatch
	code 80020005

pow:
	variant_pow(hello, 3.5)
	exception Type mismatch
	code 80020005

xor:
	variant_xor(hello, 3.5)
	exception Type mismatch
	code 80020005

--
add: hellohello
cat: hellohello
sub:
	variant_sub(hello, hello)
	exception Type mismatch
	code 80020005

mul:
	variant_mul(hello, hello)
	exception Type mismatch
	code 80020005

and:
	variant_and(hello, hello)
	exception Type mismatch
	code 80020005

div:
	variant_div(hello, hello)
	exception Type mismatch
	code 80020005

eqv:
	variant_eqv(hello, hello)
	exception Type mismatch
	code 80020005

idiv:
	variant_idiv(hello, hello)
	exception Type mismatch
	code 80020005

imp:
	variant_imp(hello, hello)
	exception Type mismatch
	code 80020005

mod:
	variant_mod(hello, hello)
	exception Type mismatch
	code 80020005

or:
	variant_or(hello, hello)
	exception Type mismatch
	code 80020005

pow:
	variant_pow(hello, hello)
	exception Type mismatch
	code 80020005

xor:
	variant_xor(hello, hello)
	exception Type mismatch
	code 80020005

--
add:
	variant_add(hello, )
	exception Type mismatch
	code 80020005

cat: helloFalse
sub:
	variant_sub(hello, )
	exception Type mismatch
	code 80020005

mul:
	variant_mul(hello, )
	exception Type mismatch
	code 80020005

and:
	variant_and(hello, )
	exception Type mismatch
	code 80020005

div:
	variant_div(hello, )
	exception Type mismatch
	code 80020005

eqv:
	variant_eqv(hello, )
	exception Type mismatch
	code 80020005

idiv:
	variant_idiv(hello, )
	exception Type mismatch
	code 80020005

imp:
	variant_imp(hello, )
	exception Type mismatch
	code 80020005

mod:
	variant_mod(hello, )
	exception Type mismatch
	code 80020005

or:
	variant_or(hello, )
	exception Type mismatch
	code 80020005

pow:
	variant_pow(hello, )
	exception Type mismatch
	code 80020005

xor:
	variant_xor(hello, )
	exception Type mismatch
	code 80020005

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
	variant_add(0, hello)
	exception Type mismatch
	code 80020005

cat: Falsehello
sub:
	variant_sub(0, hello)
	exception Type mismatch
	code 80020005

mul:
	variant_mul(0, hello)
	exception Type mismatch
	code 80020005

and:
	variant_and(0, hello)
	exception Type mismatch
	code 80020005

div:
	variant_div(0, hello)
	exception Type mismatch
	code 80020005

eqv:
	variant_eqv(0, hello)
	exception Type mismatch
	code 80020005

idiv:
	variant_idiv(0, hello)
	exception Type mismatch
	code 80020005

imp:
	variant_imp(0, hello)
	exception Type mismatch
	code 80020005

mod:
	variant_mod(0, hello)
	exception Type mismatch
	code 80020005

or:
	variant_or(0, hello)
	exception Type mismatch
	code 80020005

pow:
	variant_pow(0, hello)
	exception Type mismatch
	code 80020005

xor:
	variant_xor(0, hello)
	exception Type mismatch
	code 80020005

--
add: 0
cat: FalseFalse
sub: 0
mul: 0
and: 0
div:
	variant_div(0, )
	exception Out of present range
	code 8002000a

eqv: -1
idiv:
	variant_idiv(0, )
	exception Division by zero
	code 80020012

imp: -1
mod:
	variant_mod(0, )
	exception Division by zero
	code 80020012

or: 0
pow: 1
xor: 0
OK!
