--TEST--
A compound assignment that throws must leave a typed property or reference unchanged
--FILE--
<?php

class C {
    public int $i = 5;
    public float $f = 2.5;
    public ?int $n = 7;
    public static int $s = 5;
}

class S {
    public function __toString(): string { throw new Exception('toString'); }
}

function test(string $label, Closure $op, Closure $get) {
    try {
        $op();
        echo "$label: no exception\n";
    } catch (Throwable $e) {
        echo "$label: ", get_class($e), "\n";
    }
    var_dump($get());
}

$o = new C;
test('int += []', function () use ($o) { $o->i += []; }, fn () => $o->i);
test('int /= 0', function () use ($o) { $o->i /= 0; }, fn () => $o->i);
test('int %= 0', function () use ($o) { $o->i %= 0; }, fn () => $o->i);
test('int <<= -1', function () use ($o) { $o->i <<= -1; }, fn () => $o->i);
test('int -= "abc"', function () use ($o) { $o->i -= "abc"; }, fn () => $o->i);
test('int .= S', function () use ($o) { $o->i .= new S; }, fn () => $o->i);
test('float += []', function () use ($o) { $o->f += []; }, fn () => $o->f);
test('?int /= 0', function () use ($o) { $o->n /= 0; }, fn () => $o->n);
test('static int /= 0', function () { C::$s /= 0; }, fn () => C::$s);

$r = &$o->i;
test('typed ref /= 0', function () use (&$r) { $r /= 0; }, fn () => $o->i);
test('typed ref += []', function () use (&$r) { $r += []; }, fn () => $o->i);
unset($r);

test('int += 1', function () use ($o) { $o->i += 1; }, fn () => $o->i);

set_error_handler(function ($no, $msg) { throw new ErrorException($msg, 0, $no); });
$o = new C;
test('int += "5 apples" with a throwing error handler', function () use ($o) { $o->i += "5 apples"; }, fn () => $o->i);
test('int %= 1.5 with a throwing error handler', function () use ($o) { $o->i %= 1.5; }, fn () => $o->i);
restore_error_handler();

?>
--EXPECT--
int += []: TypeError
int(5)
int /= 0: DivisionByZeroError
int(5)
int %= 0: DivisionByZeroError
int(5)
int <<= -1: ArithmeticError
int(5)
int -= "abc": TypeError
int(5)
int .= S: Exception
int(5)
float += []: TypeError
float(2.5)
?int /= 0: DivisionByZeroError
int(7)
static int /= 0: DivisionByZeroError
int(5)
typed ref /= 0: DivisionByZeroError
int(5)
typed ref += []: TypeError
int(5)
int += 1: no exception
int(6)
int += "5 apples" with a throwing error handler: ErrorException
int(5)
int %= 1.5 with a throwing error handler: ErrorException
int(5)
