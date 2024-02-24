--TEST--
NodeList dimensions
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root><a/><b/><c/></root>');
$children = $dom->documentElement->childNodes;

$test_values = [-1, 0, 1, 2, 3, 1.0, 1.1, true, null, "0", ""];

foreach ($test_values as $value) {
    echo "--- ", json_encode($value), " ---\n";
    try {
        var_dump($children[$value] ? $children[$value]->nodeName : "N/A", isset($children[$value]), empty($children[$value]));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
--- -1 ---
string(3) "N/A"
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
Cannot access offset of type bool on DOM\NodeList
--- null ---
Cannot access offset of type null on DOM\NodeList
--- "0" ---
Cannot access offset of type string on DOM\NodeList
--- "" ---
Cannot access offset of type string on DOM\NodeList
