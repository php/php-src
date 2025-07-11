--TEST--
Test variations in usage of acosh()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = acosh($value);
    var_dump($res);
}

?>
--EXPECT--
float(3.8281684713331012)
float(NAN)
float(3.8475627390640357)
float(3.8475627390640357)
float(NAN)
float(2.993222846126381)
float(3.0229262125659933)
float(8.974744596703715)
