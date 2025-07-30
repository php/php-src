--TEST--
Test is_representable_as_int() function
--FILE--
<?php
$representable_as_int = array(
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
    42.0,
    -42.0,
    
    "0",
    "1",
    "-1",
    "42",
    "-42",
    "123456789",
    "-123456789",
    
    "0.0",
    "1.0",
    "-1.0",
    "42.0",
    "-42.0",
    
    "1e2",
    "1e3",
    "-1e2",
    
    " 42 ",
    "\t100\n",
    "\r-123\v",
);

foreach ($representable_as_int as $value) {
    var_dump(is_representable_as_int($value));
}

$not_representable_as_int = array(
    0.1,
    -0.1,
    3.14,
    -3.14,
    1.5,
    -1.5,
    
    INF,
    -INF,
    NAN,
    
    1e20,   // too large for int
    -1e20,  // too small for int
    
    null,
    true,
    false,
    "",
    "hello",
    "123.456.789",
    
    [],
    [1, 2, 3],
    new stdClass(),
    
    "3.14",
    "-3.14",
    "1.5",
    "-1.5",
    "0.1",
    "-0.1",
    
    "1.23e1",
    "1e-1",
    "-1e-1",
    
    "0x42",
    "0xFF",
    
    "0b1010",
);

$fp = fopen(__FILE__, "r");
$not_representable_as_int[] = $fp; // resource

foreach ($not_representable_as_int as $value) {
    var_dump(is_representable_as_int($value));
}

class TestClass {
    public function __toString() {
        return "42";
    }
}

$obj = new TestClass();
var_dump(is_representable_as_int($obj));

class TestClass2 {
    public function __toString() {
        return "3.14";
    }
}

$obj2 = new TestClass2();
var_dump(is_representable_as_int($obj2));

class TestClass3 {
    public function __toString() {
        return "not_numeric";
    }
}

$obj3 = new TestClass3();
var_dump(is_representable_as_int($obj3));

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
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
