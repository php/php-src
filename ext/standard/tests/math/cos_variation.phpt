--TEST--
Test variations in usage of cos()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = cos($value);
    var_dump($res);
}

?>
--EXPECT--
float(-0.5328330203333975)
float(-0.5328330203333975)
float(-0.11171123911915933)
float(-0.11171123911915933)
float(-0.11171123911915933)
float(-0.8390715290764524)
float(-0.6408264175949933)
float(-0.052737412665270565)
