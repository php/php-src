--TEST--
Inherited methods: substituted return type is enforced even when the parent's bound is mixed
--FILE--
<?php
class Box<T> {
    public function makeArr(): T { return [1, 2]; }
    public function makeNonNumericStr(): T { return "hello"; }
    public function makeInt(): T { return 42; }
}

class IntBox extends Box<int> {}
class StringBox extends Box<string> {}

// Array can't coerce to int.
try {
    (new IntBox())->makeArr();
} catch (TypeError $e) {
    echo "intbox-arr: ", $e->getMessage(), "\n";
}

// Non-numeric string can't coerce to int.
try {
    (new IntBox())->makeNonNumericStr();
} catch (TypeError $e) {
    echo "intbox-str: ", $e->getMessage(), "\n";
}

// Array can't coerce to string.
try {
    (new StringBox())->makeArr();
} catch (TypeError $e) {
    echo "strbox-arr: ", $e->getMessage(), "\n";
}

// Returns that happen to satisfy the substituted type pass.
var_dump((new IntBox())->makeInt());
var_dump((new StringBox())->makeNonNumericStr());

// Parent's own calls with the mixed bound accept everything.
var_dump((new Box())->makeNonNumericStr());
var_dump((new Box())->makeInt());
var_dump((new Box())->makeArr());
?>
--EXPECT--
intbox-arr: Box::makeArr(): Return value must be of type int, array returned
intbox-str: Box::makeNonNumericStr(): Return value must be of type int, string returned
strbox-arr: Box::makeArr(): Return value must be of type string, array returned
int(42)
string(5) "hello"
string(5) "hello"
int(42)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
