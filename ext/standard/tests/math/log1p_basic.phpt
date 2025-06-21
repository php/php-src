--TEST--
Test log1p() - basic function test log1p()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    echo "log1p($value): ";
    var_dump(log1p($value));
};


?>
--EXPECT--
log1p(23): float(3.1780538303479458)
log1p(-23): float(NAN)
log1p(23.45): float(3.196630215920881)
log1p(23.45): float(3.196630215920881)
log1p(-23.45): float(NAN)
log1p(10): float(2.3978952727983707)
log1p(10.3): float(2.424802725718295)
log1p(3950.5): float(8.28185053264503)
