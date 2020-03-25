--TEST--
Bug #73329 (Float)"Nano" == NAN
--FILE--
<?php
    var_dump(
        (float)"nanite",
        (float)"nan",
        (float)"inf",
        (float)"infusorian"
    );
?>
--EXPECT--
float(0)
float(0)
float(0)
float(0)
