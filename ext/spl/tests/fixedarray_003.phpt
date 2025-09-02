--TEST--
SPL: SplFixedArray: Non integer offset handling
--FILE--
<?php
$o = new SplFixedArray(10);
$r = fopen('php://memory', 'r+');


echo 'Write context', \PHP_EOL;
$o[false] = 'a';
$o[true] = 'b';
$o[2.5] = 'c';

try {
    $o[[]] = 'd';
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $o[new stdClass()] = 'e';
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $o[$r] = 'f';
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

$o['3'] = 'g';

try {
    $o['3.5'] = 'h';
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $o['03'] = 'i';
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $o[' 3'] = 'j';
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}


echo 'Read context', \PHP_EOL;
var_dump($o[false]);
var_dump($o[true]);
var_dump($o[2.5]);

try {
    var_dump($o[[]]);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($o[new stdClass()]);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($o[$r]);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

var_dump($o['3']);

try {
    var_dump($o['3.5']);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($o['03']);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($o[' 3']);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}


echo 'isset()', \PHP_EOL;
var_dump(isset($o[false]));
var_dump(isset($o[true]));
var_dump(isset($o[2.5]));

try {
    var_dump(isset($o[[]]));
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(isset($o[new stdClass()]));
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(isset($o[$r]));
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

var_dump(isset($o['3']));

try {
    var_dump(isset($o['3.5']));
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(isset($o['03']));
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(isset($o[' 3']));
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo 'empty()', \PHP_EOL;
var_dump(empty($o[false]));
var_dump(empty($o[true]));
var_dump(empty($o[2.5]));

try {
    var_dump(empty($o[[]]));
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(empty($o[new stdClass()]));
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(empty($o[$r]));
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

var_dump(empty($o['3']));

try {
    var_dump(empty($o['3.5']));
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(empty($o['03']));
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(empty($o[' 3']));
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Write context

Deprecated: Implicit conversion from float 2.5 to int loses precision in %s on line %d
Cannot access offset of type array on SplFixedArray
Cannot access offset of type stdClass on SplFixedArray

Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
Cannot access offset of type string on SplFixedArray
Cannot access offset of type string on SplFixedArray
Cannot access offset of type string on SplFixedArray
Read context
string(1) "a"
string(1) "b"

Deprecated: Implicit conversion from float 2.5 to int loses precision in %s on line %d
string(1) "c"
Cannot access offset of type array on SplFixedArray
Cannot access offset of type stdClass on SplFixedArray

Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
string(1) "f"
string(1) "g"
Cannot access offset of type string on SplFixedArray
Cannot access offset of type string on SplFixedArray
Cannot access offset of type string on SplFixedArray
isset()
bool(true)
bool(true)

Deprecated: Implicit conversion from float 2.5 to int loses precision in %s on line %d
bool(true)
Cannot access offset of type array on SplFixedArray
Cannot access offset of type stdClass on SplFixedArray

Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
bool(true)
bool(true)
Cannot access offset of type string on SplFixedArray
Cannot access offset of type string on SplFixedArray
Cannot access offset of type string on SplFixedArray
empty()
bool(false)
bool(false)

Deprecated: Implicit conversion from float 2.5 to int loses precision in %s on line %d
bool(false)
Cannot access offset of type array on SplFixedArray
Cannot access offset of type stdClass on SplFixedArray

Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
bool(false)
bool(false)
Cannot access offset of type string on SplFixedArray
Cannot access offset of type string on SplFixedArray
Cannot access offset of type string on SplFixedArray
