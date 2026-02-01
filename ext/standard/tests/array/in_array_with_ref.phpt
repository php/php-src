--TEST--
in_array() with references
--FILE--
<?php

enum Sample {
    case A;
    case B;
}

$refVar = "B";
$refVar2 = 4;
$refVar3 = -2.989888;
$refVar4 = NULL;
$refVar5 = Sample::A;
$refVar6 = [];
$refVar7 = True;
$array = [&$refVar2, &$refVar, &$refVar3, &$refVar4, &$refVar5, &$refVar6, &$refVar7];

$array_compare = array (
  4,
  "4",
  4.00,
  "b",
  "5",
  -2,
  -2.0,
  -2.98989,
  "-.9",
  "True",
  "",
  [],
  NULL,
  "ab",
  "abcd",
  0.0,
  -0,
  "abcd\x00abcd\x00abcd",
  Sample::A,
  Sample::B,
);

function get_type(mixed $value): string {
    if (is_object($value)) {
        return var_export($value, true);
    }
    return gettype($value). ':' . json_encode($value, true);
}

foreach($array_compare as $compare) {
    echo "-- Iteration -- ".get_type($compare)."\n";
    //strict option OFF
    var_dump(in_array($compare,$array));
    //strict option ON
    var_dump(in_array($compare,$array,TRUE));
    //strict option OFF
    var_dump(in_array($compare,$array,FALSE));
}

echo "Done\n";
?>
--EXPECT--
-- Iteration -- integer:4
bool(true)
bool(true)
bool(true)
-- Iteration -- string:"4"
bool(true)
bool(false)
bool(true)
-- Iteration -- double:4
bool(true)
bool(false)
bool(true)
-- Iteration -- string:"b"
bool(true)
bool(false)
bool(true)
-- Iteration -- string:"5"
bool(true)
bool(false)
bool(true)
-- Iteration -- integer:-2
bool(true)
bool(false)
bool(true)
-- Iteration -- double:-2
bool(true)
bool(false)
bool(true)
-- Iteration -- double:-2.98989
bool(true)
bool(false)
bool(true)
-- Iteration -- string:"-.9"
bool(true)
bool(false)
bool(true)
-- Iteration -- string:"True"
bool(true)
bool(false)
bool(true)
-- Iteration -- string:""
bool(true)
bool(false)
bool(true)
-- Iteration -- array:[]
bool(true)
bool(true)
bool(true)
-- Iteration -- NULL:null
bool(true)
bool(true)
bool(true)
-- Iteration -- string:"ab"
bool(true)
bool(false)
bool(true)
-- Iteration -- string:"abcd"
bool(true)
bool(false)
bool(true)
-- Iteration -- double:0
bool(true)
bool(false)
bool(true)
-- Iteration -- integer:0
bool(true)
bool(false)
bool(true)
-- Iteration -- string:"abcd\u0000abcd\u0000abcd"
bool(true)
bool(false)
bool(true)
-- Iteration -- \Sample::A
bool(true)
bool(true)
bool(true)
-- Iteration -- \Sample::B
bool(true)
bool(false)
bool(true)
Done
