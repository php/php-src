--TEST--
Testing isset with string offsets
--FILE--
<?php

print "- isset ---\n";

$str = "test0123";

var_dump(isset($str[-1]));
var_dump(isset($str[-10]));
var_dump(isset($str[0]));
var_dump(isset($str[1]));
var_dump(isset($str[4])); // 0
var_dump(isset($str[5])); // 1
var_dump(isset($str[8]));
var_dump(isset($str[10000]));
// non-numeric offsets
print "- string literal ---\n";
var_dump(isset($str['-1'])); // 3
var_dump(isset($str['-10']));
var_dump(isset($str['0']));
var_dump(isset($str['1']));
var_dump(isset($str['4'])); // 0
try {
    var_dump(isset($str['1.5']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(isset($str['good']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(isset($str['3 and a half']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
print "- string variable ---\n";
var_dump(isset($str[$key = '-1'])); // 3
var_dump(isset($str[$key = '-10']));
var_dump(isset($str[$key = '0']));
var_dump(isset($str[$key = '1']));
var_dump(isset($str[$key = '4'])); // 0
try {
    var_dump(isset($str[$key = '1.5']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(isset($str[$key = 'good']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(isset($str[$key = '3 and a half']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
print "- bool ---\n";
var_dump(isset($str[true]));
var_dump(isset($str[false]));
echo "Sub-keys:\n";
var_dump(isset($str[false][true]));
print "- null ---\n";
var_dump(isset($str[null]));
print "- double ---\n";
var_dump(isset($str[-1.1]));
var_dump(isset($str[-10.5]));
var_dump(isset($str[-0.8]));
var_dump(isset($str[-0.1]));
var_dump(isset($str[0.2]));
var_dump(isset($str[0.9]));
var_dump(isset($str[M_PI]));
var_dump(isset($str[100.5001]));
print "- array ---\n";
try {
    var_dump(isset($str[array()]));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(isset($str[array(1,2,3)]));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
print "- object ---\n";
try {
    var_dump(isset($str[new stdClass()]));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
print "- resource ---\n";
$f = fopen(__FILE__, 'r');
try {
    var_dump(isset($str[$f]));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
print "done\n";

?>
--EXPECTF--
- isset ---
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
- string literal ---
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)

Warning: Cannot access offset of type string in isset or empty in %s on line %d
bool(false)

Warning: Cannot access offset of type string in isset or empty in %s on line %d
bool(false)

Warning: Illegal string offset "3 and a half" in %s on line %d
bool(false)
- string variable ---
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)

Warning: Cannot access offset of type string in isset or empty in %s on line %d
bool(false)

Warning: Cannot access offset of type string in isset or empty in %s on line %d
bool(false)

Warning: Illegal string offset "3 and a half" in %s on line %d
bool(false)
- bool ---

Warning: String offset cast occurred in %s on line %d
bool(true)

Warning: String offset cast occurred in %s on line %d
bool(true)
Sub-keys:

Warning: String offset cast occurred in %s on line %d

Warning: String offset cast occurred in %s on line %d
bool(false)
- null ---

Warning: String offset cast occurred in %s on line %d
bool(true)
- double ---

Warning: String offset cast occurred in %s on line %d
bool(true)

Warning: String offset cast occurred in %s on line %d
bool(false)

Warning: String offset cast occurred in %s on line %d
bool(true)

Warning: String offset cast occurred in %s on line %d
bool(true)

Warning: String offset cast occurred in %s on line %d
bool(true)

Warning: String offset cast occurred in %s on line %d
bool(true)

Warning: String offset cast occurred in %s on line %d
bool(true)

Warning: String offset cast occurred in %s on line %d
bool(false)
- array ---

Warning: Cannot access offset of type array in isset or empty in %s on line %d
bool(false)

Warning: Cannot access offset of type array in isset or empty in %s on line %d
bool(false)
- object ---

Warning: Cannot access offset of type stdClass in isset or empty in %s on line %d
bool(false)
- resource ---

Warning: Cannot access offset of type resource in isset or empty in %s on line %d
bool(false)
done
