--TEST--
Test in_array() function : usage variations - haystack with mixed string/int keys
--FILE--
<?php
include(__DIR__ . '/in_array_variation1_data.inc');

$haystack = ["a" => "A", 2 => "B", "C" => 3, 4 => 4, "one" => 1, "" => NULL, "b", "ab", "abcd"];

foreach ($values as $desc => $needle) {
    echo "-- $desc --\n";
    echo "in_array() strict=false\n";
    var_dump(in_array($needle, $haystack));
    echo "in_array() strict=true\n";
    var_dump(in_array($needle, $haystack, TRUE));
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
bool(true)
-- string '5' --
in_array() strict=false
bool(false)
in_array() strict=true
bool(false)
-- integer -2 --
in_array() strict=false
bool(false)
in_array() strict=true
bool(false)
-- float -2.0 --
in_array() strict=false
bool(false)
in_array() strict=true
bool(false)
-- float -2.98989 --
in_array() strict=false
bool(false)
in_array() strict=true
bool(false)
-- string '-.9' --
in_array() strict=false
bool(false)
in_array() strict=true
bool(false)
-- string 'True' --
in_array() strict=false
bool(false)
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
bool(false)
-- null --
in_array() strict=false
bool(true)
in_array() strict=true
bool(true)
-- string 'ab' --
in_array() strict=false
bool(true)
in_array() strict=true
bool(true)
-- string 'abcd' --
in_array() strict=false
bool(true)
in_array() strict=true
bool(true)
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
bool(false)
in_array() strict=true
bool(false)
-- enum Sample::A --
in_array() strict=false
bool(false)
in_array() strict=true
bool(false)
-- enum Sample::B --
in_array() strict=false
bool(false)
in_array() strict=true
bool(false)
