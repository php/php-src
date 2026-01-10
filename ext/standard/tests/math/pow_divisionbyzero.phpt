--TEST--
pow() division by zero error
--FILE--
<?php

function test() {
    var_dump(pow(0, -0));
    var_dump(pow(0.0, -0));
    var_dump(pow(0, -0.0));
    var_dump(pow(0.0, -0.0));
    var_dump(pow(0, -0.01));
    var_dump(pow(0.0, -0.01));
    var_dump(pow(0, -1));
    var_dump(pow(0.0, -1));
    var_dump(pow(0, -1.1));
    var_dump(pow(0.0, -1.1));

    var_dump(0 ** -0);
    var_dump(0.0 ** -0);
    var_dump(0 ** -0.0);
    var_dump(0.0 ** -0.0);
    var_dump(0 ** -0.01);
    var_dump(0.0 ** -0.01);
    var_dump(0 ** -1);
    var_dump(0.0 ** -1);
    var_dump(0 ** -1.1);
    var_dump(0.0 ** -1.1);
}

test();
test();

?>
--EXPECTF--
int(1)
float(1)
float(1)
float(1)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)
int(1)
float(1)
float(1)
float(1)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)
int(1)
float(1)
float(1)
float(1)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)
int(1)
float(1)
float(1)
float(1)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)

Deprecated: Power of base 0 and negative exponent is deprecated in %s on line %d
float(INF)
