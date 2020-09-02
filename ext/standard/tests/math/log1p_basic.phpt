--TEST--
Test log1p() - basic function test log1p()
--INI--
precision=14
--FILE--
<?php
echo "*** Testing log1p() : basic functionality ***\n";

$values = array(23,
                -23,
                2.345e1,
                -2.345e1,
                0x17,
                027,
                "23",
                "23.45",
                "2.345e1",
                null,
                true,
                false);

echo "\n LOG1p tests\n";

foreach($values as $value) {
    echo "\n-- log1p $value --\n";
    var_dump(log1p($value));
};


?>
--EXPECT--
*** Testing log1p() : basic functionality ***

 LOG1p tests

-- log1p 23 --
float(3.1780538303479458)

-- log1p -23 --
float(NAN)

-- log1p 23.45 --
float(3.196630215920881)

-- log1p -23.45 --
float(NAN)

-- log1p 23 --
float(3.1780538303479458)

-- log1p 23 --
float(3.1780538303479458)

-- log1p 23 --
float(3.1780538303479458)

-- log1p 23.45 --
float(3.196630215920881)

-- log1p 2.345e1 --
float(3.196630215920881)

-- log1p  --
float(0)

-- log1p 1 --
float(0.6931471805599453)

-- log1p  --
float(0)
