--TEST--
Test variations in usage of tan()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = tan($value);
    var_dump($res);
}

?>
--EXPECT--
float(1.5881530833912738)
float(-1.5881530833912738)
float(8.895619796255806)
float(8.895619796255806)
float(-8.895619796255806)
float(0.6483608274590866)
float(1.1979621761610415)
float(18.935483633520395)
