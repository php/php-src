--TEST--
Test is_nan() - basic function test is_nan()
--FILE--
<?php
$values = array(234,
                -234,
                23.45e1,
                -23.45e1,
                0xEA,
                0352,
                "234",
                "234.5",
                "23.45e1",
                true,
                false,
                pow(0, -2),
                acos(1.01));


for ($i = 0; $i < count($values); $i++) {
    $res = is_nan($values[$i]);
    var_dump($res);
}

?>
--EXPECT--
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
