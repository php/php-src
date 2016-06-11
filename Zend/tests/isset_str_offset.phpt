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
print "- string ---\n";
var_dump(isset($str['-1']));
var_dump(isset($str['-10']));
var_dump(isset($str['0']));
var_dump(isset($str['1']));
var_dump(isset($str['4'])); // 0
var_dump(isset($str['1.5']));
var_dump(isset($str['good']));
var_dump(isset($str['3 and a half']));
print "- bool ---\n";
var_dump(isset($str[true]));
var_dump(isset($str[false]));
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
var_dump(isset($str[array()]));
var_dump(isset($str[array(1,2,3)]));
print "- object ---\n";
var_dump(isset($str[new stdClass()]));
print "- resource ---\n";
$f = fopen(__FILE__, 'r');
var_dump(isset($str[$f]));
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
- string ---
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
- bool ---
bool(true)
bool(true)
bool(false)
- null ---
bool(true)
- double ---
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
- array ---
bool(false)
bool(false)
- object ---
bool(false)
- resource ---
bool(false)
done
