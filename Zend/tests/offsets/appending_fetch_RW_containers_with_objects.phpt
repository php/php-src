--TEST--
Appending containers via a binary RW op $c[] .= $v; fetch is an object
--FILE--
<?php

class FakeString {
    public function __toString(): string {
        return "Hello ";
    }
}

class TestFetchAppendStringableObject implements FetchAppendable {
    public object $str;
    public function append(mixed $v): void {}
    public function &fetchAppend(): object {
        $this->str = new FakeString();
        return $this->str;
    }
}

$container = new TestFetchAppendStringableObject();
try {
    $container[] .= 'value';
    var_dump($container);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

class TestFetchAppendObject implements FetchAppendable {
    public object $n;
    public function append(mixed $v): void {}
    public function &fetchAppend(): object {
        $this->n = gmp_init(20);
        $ref = $this->n;
        return $ref;
    }
}

$container = new TestFetchAppendObject();
try {
    $container[] += 22;
    var_dump($container);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
NULL container:
array(1) {
  [0]=>
  string(5) "value"
}
false container:

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
array(1) {
  [0]=>
  string(5) "value"
}
true container:
Error: Cannot use a scalar value as an array
4 container:
Error: Cannot use a scalar value as an array
5.5 container:
Error: Cannot use a scalar value as an array
'10' container:
Error: [] operator not supported for strings
'25.5' container:
Error: [] operator not supported for strings
'string' container:
Error: [] operator not supported for strings
[] container:
array(1) {
  [0]=>
  string(5) "value"
}
STDERR container:
Error: Cannot use a scalar value as an array
new stdClass() container:
Error: Cannot use object of type stdClass as array
new ArrayObject() container:
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    [0]=>
    string(5) "value"
  }
}
new A() container:
string(12) "A::offsetGet"
NULL

Notice: Indirect modification of overloaded element of A has no effect in %s on line %d
object(A)#3 (0) {
}
new B() container:
object(B)#4 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    [0]=>
    string(5) "value"
  }
}
