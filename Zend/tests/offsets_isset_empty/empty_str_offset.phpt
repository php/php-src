--TEST--
Testing empty() with string offsets
--FILE--
<?php

print "- empty ---\n";

$str = "test0123";

var_dump(empty($str[-1]));
var_dump(empty($str[-10]));
var_dump(empty($str[-4])); // 0
var_dump(empty($str[0]));
var_dump(empty($str[1]));
var_dump(empty($str[4])); // 0
var_dump(empty($str[5])); // 1
var_dump(empty($str[8]));
var_dump(empty($str[10000]));
// non-numeric offsets
print "- string literal ---\n";
var_dump(empty($str['-1'])); // 3
var_dump(empty($str['-10']));
var_dump(empty($str['0']));
var_dump(empty($str['1']));
var_dump(empty($str['4'])); // 0
try {
    var_dump(empty($str['1.5']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(empty($str['good']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(empty($str['3 and a half']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
print "- string variable ---\n";
var_dump(empty($str[$key = '-1'])); // 3
var_dump(empty($str[$key = '-10']));
var_dump(empty($str[$key = '0']));
var_dump(empty($str[$key = '1']));
var_dump(empty($str[$key = '4'])); // 0
try {
    var_dump(empty($str[$key = '1.5']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(empty($str[$key = 'good']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(empty($str[$key = '3 and a half']));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
print "- bool ---\n";
var_dump(empty($str[true]));
var_dump(empty($str[false]));
echo "Sub-keys:\n";
var_dump(empty($str[false][true]));
print "- null ---\n";
var_dump(empty($str[null]));
print "- double ---\n";
var_dump(empty($str[-1.1]));
var_dump(empty($str[-10.5]));
var_dump(empty($str[-4.1]));
var_dump(empty($str[-0.8]));
var_dump(empty($str[-0.1]));
var_dump(empty($str[0.2]));
var_dump(empty($str[0.9]));
var_dump(empty($str[M_PI]));
var_dump(empty($str[100.5001]));
print "- array ---\n";
try {
    var_dump(empty($str[array()]));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(empty($str[array(1,2,3)]));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
print "- object ---\n";
try {
    var_dump(empty($str[new stdClass()]));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
print "- resource ---\n";
$f = fopen(__FILE__, 'r');
try {
    var_dump(empty($str[$f]));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
print "done\n";

?>
--EXPECTF--
- empty ---
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
- string literal ---
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)

Warning: Cannot access offset of type string in isset or empty in %s on line %d
bool(true)

Warning: Cannot access offset of type string in isset or empty in %s on line %d
bool(true)

Warning: Illegal string offset "3 and a half" in %s on line %d
bool(true)
- string variable ---
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)

Warning: Cannot access offset of type string in isset or empty in %s on line %d
bool(true)

Warning: Cannot access offset of type string in isset or empty in %s on line %d
bool(true)

Warning: Illegal string offset "3 and a half" in %s on line %d
bool(true)
- bool ---

Warning: String offset cast occurred in %s on line %d
bool(false)

Warning: String offset cast occurred in %s on line %d
bool(false)
Sub-keys:

Warning: String offset cast occurred in %s on line %d

Warning: String offset cast occurred in %s on line %d
bool(true)
- null ---

Warning: String offset cast occurred in %s on line %d
bool(false)
- double ---

Warning: String offset cast occurred in %s on line %d
bool(false)

Warning: String offset cast occurred in %s on line %d
bool(true)

Warning: String offset cast occurred in %s on line %d
bool(true)

Warning: String offset cast occurred in %s on line %d
bool(false)

Warning: String offset cast occurred in %s on line %d
bool(false)

Warning: String offset cast occurred in %s on line %d
bool(false)

Warning: String offset cast occurred in %s on line %d
bool(false)

Warning: String offset cast occurred in %s on line %d
bool(false)

Warning: String offset cast occurred in %s on line %d
bool(true)
- array ---

Warning: Cannot access offset of type array in isset or empty in %s on line %d
bool(true)

Warning: Cannot access offset of type array in isset or empty in %s on line %d
bool(true)
- object ---

Warning: Cannot access offset of type stdClass in isset or empty in %s on line %d
bool(true)
- resource ---

Warning: Cannot access offset of type resource in isset or empty in %s on line %d
bool(true)
done
