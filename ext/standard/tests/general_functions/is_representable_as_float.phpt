--TEST--
Test is_representable_as_float() function
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit systems only"); ?>
--FILE--
<?php
$representable_as_float = array(
    0,
    1,
    -1,
    42,
    -42,
    
    9007199254740992,  // 2^53
    -9007199254740992, // -2^53
    9007199254740991,  // 2^53 - 1
    -9007199254740991, // -(2^53 - 1)
    
    // from 2^53 to 2^54, only even numbers are exactly representable
    9007199254740994,  // 2^53 + 2
    9007199254740996,  // 2^53 + 4
    18014398509481984, // 2^54
    -9007199254740994, // -(2^53 + 2)
    
    // from 2^54 to 2^55, only multiples of 4 are exactly representable
    18014398509481988, // 2^54 + 4
    18014398509481992, // 2^54 + 8
    36028797018963968, // 2^55
    
    // from 2^55 to 2^56, only multiples of 8 are exactly representable
    36028797018963976, // 2^55 + 8
    36028797018963984, // 2^55 + 16
    72057594037927936, // 2^56
    
    // pure powers of two
    1152921504606846976, // 2^60
    2305843009213693952, // 2^61
    
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
    "9007199254740992",  // 2^53
    "-9007199254740992", // -2^53
    
    
    "9007199254740994",  // 2^53 + 2
    "18014398509481988", // 2^54 + 4
    "36028797018963976", // 2^55 + 8
    
    " 42 ",
    "\t1.5\n",
    "\r-3.14\v",
);

foreach ($representable_as_float as $value) {
    var_dump(is_representable_as_float($value));
}

$not_representable_as_float = array(
    9007199254740993,  // 2^53 + 1
    -9007199254740993, // -(2^53 + 1)
    9007199254740995,  // 2^53 + 3
    
    // odd numbers in 2^53 to 2^54 range
    9007199254740995,  // 2^53 + 3
    9007199254740997,  // 2^53 + 5
    18014398509481985, // 2^54 + 1
    
    // not multiples of 4 in 2^54 to 2^55 range
    18014398509481986, // 2^54 + 2
    18014398509481990, // 2^54 + 6
    36028797018963970, // 2^55 + 2
    
    // not multiples of 8 in 2^55 to 2^56 range
    36028797018963972, // 2^55 + 4
    36028797018963978, // 2^55 + 10
    72057594037927940, // 2^56 + 4
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
    
    
    "9007199254740993",  // 2^53 + 1
    "18014398509481986", // 2^54 + 2 (not multiple of 4)
    "36028797018963972", // 2^55 + 4 (not multiple of 8)
    
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
bool(true)
bool(false)
