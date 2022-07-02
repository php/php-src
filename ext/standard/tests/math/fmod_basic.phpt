--TEST--
Test fmod() - basic function test fmod()
--INI--
precision=14
--FILE--
<?php
$values1 = array(234,
                -234,
                23.45e1,
                -23.45e1,
                0xEA,
                0352,
                "234",
                "234.5",
                "23.45e1",
                true,
                false);

$values2 = array(2,
                -2,
                2.3e1,
                -2.3e1,
                0x2,
                02,
                "2",
                "2.3",
                "2.3e1",
                true,
                false);
for ($i = 0; $i < count($values1); $i++) {
    echo "\niteration ", $i, "\n";

    for ($j = 0; $j < count($values2); $j++) {
        $res = fmod($values1[$i], $values2[$j]);
        var_dump($res);
    }
}
?>
--EXPECT--
iteration 0
float(0)
float(0)
float(4)
float(4)
float(0)
float(0)
float(0)
float(1.700000000000018)
float(4)
float(0)
float(NAN)

iteration 1
float(-0)
float(-0)
float(-4)
float(-4)
float(-0)
float(-0)
float(-0)
float(-1.700000000000018)
float(-4)
float(-0)
float(NAN)

iteration 2
float(0.5)
float(0.5)
float(4.5)
float(4.5)
float(0.5)
float(0.5)
float(0.5)
float(2.200000000000018)
float(4.5)
float(0.5)
float(NAN)

iteration 3
float(-0.5)
float(-0.5)
float(-4.5)
float(-4.5)
float(-0.5)
float(-0.5)
float(-0.5)
float(-2.200000000000018)
float(-4.5)
float(-0.5)
float(NAN)

iteration 4
float(0)
float(0)
float(4)
float(4)
float(0)
float(0)
float(0)
float(1.700000000000018)
float(4)
float(0)
float(NAN)

iteration 5
float(0)
float(0)
float(4)
float(4)
float(0)
float(0)
float(0)
float(1.700000000000018)
float(4)
float(0)
float(NAN)

iteration 6
float(0)
float(0)
float(4)
float(4)
float(0)
float(0)
float(0)
float(1.700000000000018)
float(4)
float(0)
float(NAN)

iteration 7
float(0.5)
float(0.5)
float(4.5)
float(4.5)
float(0.5)
float(0.5)
float(0.5)
float(2.200000000000018)
float(4.5)
float(0.5)
float(NAN)

iteration 8
float(0.5)
float(0.5)
float(4.5)
float(4.5)
float(0.5)
float(0.5)
float(0.5)
float(2.200000000000018)
float(4.5)
float(0.5)
float(NAN)

iteration 9
float(1)
float(1)
float(1)
float(1)
float(1)
float(1)
float(1)
float(1)
float(1)
float(0)
float(NAN)

iteration 10
float(0)
float(0)
float(0)
float(0)
float(0)
float(0)
float(0)
float(0)
float(0)
float(0)
float(NAN)
