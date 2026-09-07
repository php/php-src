--TEST--
in_array() with references
--FILE--
<?php
include(__DIR__ . '/in_array_variation1_data.inc');

$refVar = "B";
$refVar2 = 4;
$refVar3 = -2.989888;
$refVar4 = NULL;
$refVar5 = Sample::A;
$refVar6 = [];
$refVar7 = True;
$array = [&$refVar2, &$refVar, &$refVar3, &$refVar4, &$refVar5, &$refVar6, &$refVar7];

foreach ($values as $desc => $needle) {
    echo "-- $desc --\n";
    echo "in_array() strict=false\n";
    var_dump(in_array($needle, $array));
    echo "in_array() strict=true\n";
    var_dump(in_array($needle, $array, TRUE));
}

?>
--EXPECT--
-- integer 4 --
in_array() strict=false
bool(true)
in_array() strict=true
bool(true)
-- string '4' --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- float 4.00 --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- string 'b' --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- string '5' --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- integer -2 --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- float -2.0 --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- float -2.98989 --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- string '-.9' --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- string 'True' --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- empty string --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- empty array --
in_array() strict=false
bool(true)
in_array() strict=true
bool(true)
-- null --
in_array() strict=false
bool(true)
in_array() strict=true
bool(true)
-- string 'ab' --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- string 'abcd' --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- float 0.0 --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- integer -0 --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- string with null bytes --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
-- enum Sample::A --
in_array() strict=false
bool(true)
in_array() strict=true
bool(true)
-- enum Sample::B --
in_array() strict=false
bool(true)
in_array() strict=true
bool(false)
