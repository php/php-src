--TEST--
NamedNodeMap dimensions
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root a="1" b="2" c="3"></root>');
$attributes = $dom->documentElement->attributes;

$test_values = [-1, 0, 1, 2, 3, 1.0, 1.1, true, null, "0", "", "1", "a", "b", "c", "d"];

foreach ($test_values as $value) {
    echo "--- ", json_encode($value), " ---\n";
    try {
        var_dump($attributes[$value] ? $attributes[$value]->nodeName : "N/A", isset($attributes[$value]), empty($attributes[$value]));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
--- -1 ---
string(1) "a"
bool(false)
bool(true)
--- 0 ---
string(1) "a"
bool(true)
bool(false)
--- 1 ---
string(1) "b"
bool(true)
bool(false)
--- 2 ---
string(1) "c"
bool(true)
bool(false)
--- 3 ---
string(3) "N/A"
bool(false)
bool(true)
--- 1 ---
string(1) "b"
bool(true)
bool(false)
--- 1.1 ---

Deprecated: Implicit conversion from float 1.1 to int loses precision in %s on line %d

Deprecated: Implicit conversion from float 1.1 to int loses precision in %s on line %d

Deprecated: Implicit conversion from float 1.1 to int loses precision in %s on line %d

Deprecated: Implicit conversion from float 1.1 to int loses precision in %s on line %d
string(1) "b"
bool(true)
bool(false)
--- true ---
Cannot access offset of type bool on DOM\NamedNodeMap
--- null ---
Cannot access offset of type null on DOM\NamedNodeMap
--- "0" ---
string(3) "N/A"
bool(false)
bool(true)
--- "" ---
string(3) "N/A"
bool(false)
bool(true)
--- "1" ---
string(3) "N/A"
bool(false)
bool(true)
--- "a" ---
string(1) "a"
bool(true)
bool(false)
--- "b" ---
string(1) "b"
bool(true)
bool(false)
--- "c" ---
string(1) "c"
bool(true)
bool(false)
--- "d" ---
string(3) "N/A"
bool(false)
bool(true)
