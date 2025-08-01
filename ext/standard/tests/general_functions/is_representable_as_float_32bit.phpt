--TEST--
Test is_representable_as_float() function (32-bit)
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32-bit systems only"); ?>
--FILE--
<?php
$representable_as_float = array(
    0,
    1,
    -1,
    42,
    -42,
    PHP_INT_MAX,
    PHP_INT_MIN,
    
    0.0,
    -0.0,
    1.0,
    -1.0,
    3.14,
    -3.14,
    
    INF,
    -INF,
    NAN,
    
    2,
    4,
    8,
    16,
    1024,
    
    "0",
    "1",
    "-1",
    "42",
    "0.0",
    "1.0",
    "-1.0",
    "3.14",
    "-3.14",
    "1e10",
    "-1e10",
    "1.5e-10",
    
    " 42 ",
    "\t1.5\n",
    "\r-3.14\v",
);

foreach ($representable_as_float as $value) {
    var_dump(is_representable_as_float($value));
}

$not_representable_as_float = array(
    null,
    true,
    false,
    "",
    "hello",
    "123.456.789",
    "1e400",
    "-1e400",
    
    [],
    [1, 2, 3],
    new stdClass(),
    
    "0x42",
    "0xFF",
    
    "0b1010",
);

$fp = fopen(__FILE__, "r");
$not_representable_as_float[] = $fp;

foreach ($not_representable_as_float as $value) {
    var_dump(is_representable_as_float($value));
}

class TestClass {
    public function __toString() {
        return "42";
    }
}

$obj = new TestClass();
var_dump(is_representable_as_float($obj));

class TestClass2 {
    public function __toString() {
        return "not_numeric";
    }
}

$obj2 = new TestClass2();
var_dump(is_representable_as_float($obj2));

fclose($fp);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)