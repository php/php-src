--TEST--
Bug #76093 (NumberFormatter::format loses precision)
--EXTENSIONS--
intl
--FILE--
<?php

class Bug76093Stringable implements Stringable {
    public function __construct( private readonly string $string ) {}

    public function __toString(): string {
        return $this->string;
    }
}

# See also https://phabricator.wikimedia.org/T268456
$x = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
foreach ([
    '999999999999999999', # Fits in signed 64-bit integer
    '9999999999999999999', # Does not fit in signed 64-bit integer
    9999999999999999999, # Precision loss seen when passing as number
    ] as $value) {
    try {
        var_dump([
            'input' => $value,
            'default' => $x->format($value),
            # Note that TYPE_INT64 isn't actually guaranteed to have an
            # 64-bit integer as input, because PHP on 32-bit platforms only
            # has 32-bit integers.  If you pass the value as a string, PHP
            # will use the TYPE_DECIMAL type in order to extend the range.
            # Also, casting from double to int64 when the int64 range
            # is exceeded results in an implementation-defined value.
            'int64' => $x->format($value, NumberFormatter::TYPE_INT64),
            'double' => $x->format($value, NumberFormatter::TYPE_DOUBLE),
            'decimal' => $x->format($value, NumberFormatter::TYPE_DECIMAL),
        ]);
    } catch (TypeError $ex) {
        echo $ex->getMessage(), PHP_EOL;
    }
}

# Stringable object also supported
try {
    echo $x->format(new Bug76093Stringable('9999999999999999999')), PHP_EOL;
} catch (TypeError $ex) {
    echo $ex->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
array(5) {
  ["input"]=>
  string(18) "999999999999999999"
  ["default"]=>
  string(23) "999,999,999,999,999,999"
  ["int64"]=>
  string(23) "999,999,999,999,999,999"
  ["double"]=>
  string(25) "1,000,000,000,000,000,000"
  ["decimal"]=>
  string(23) "999,999,999,999,999,999"
}

Deprecated: Implicit conversion from float-string "9999999999999999999" to int loses precision in %s on line %d
array(5) {
  ["input"]=>
  string(19) "9999999999999999999"
  ["default"]=>
  string(25) "9,999,999,999,999,999,999"
  ["int64"]=>
  string(%d) "%r9,223,372,036,854,775,807|9,999,999,999,999,999,999%r"
  ["double"]=>
  string(26) "10,000,000,000,000,000,000"
  ["decimal"]=>
  string(25) "9,999,999,999,999,999,999"
}

Deprecated: Implicit conversion from float 1.0E+19 to int loses precision in %s on line %d
array(5) {
  ["input"]=>
  float(1.0E+19)
  ["default"]=>
  string(26) "10,000,000,000,000,000,000"
  ["int64"]=>
  string(%d) "%r[+-]?[0-9,]+%r"
  ["double"]=>
  string(26) "10,000,000,000,000,000,000"
  ["decimal"]=>
  string(26) "10,000,000,000,000,000,000"
}
9,999,999,999,999,999,999
