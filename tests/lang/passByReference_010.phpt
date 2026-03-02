--TEST--
Passing assignments by reference
--FILE--
<?php

function f(&$a) {
  var_dump($a);
  $a = "a.changed";
}

echo "\n\n---> Pass constant assignment by reference:\n";
try {
    f($a="a.original");
    var_dump($a);
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() ."\n";
}

echo "\n\n---> Pass variable assignment by reference:\n";
try {
    unset($a);
    $a = "a.original";
    f($b = $a);
    var_dump($a);
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() ."\n";
}

echo "\n\n---> Pass reference assignment by reference:\n";
try {
    unset($a, $b);
    $a = "a.original";
    f($b =& $a);
    var_dump($a);
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() ."\n";
}

echo "\n\n---> Pass concat assignment by reference:\n";
try {
    unset($a, $b);
    $b = "b.original";
    $a = "a.original";
    f($b .= $a);
    var_dump($a);
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() ."\n";
}

?>
--EXPECT--
---> Pass constant assignment by reference:
Exception: f(): Argument #1 ($a) could not be passed by reference


---> Pass variable assignment by reference:
Exception: f(): Argument #1 ($a) could not be passed by reference


---> Pass reference assignment by reference:
string(10) "a.original"
string(9) "a.changed"


---> Pass concat assignment by reference:
Exception: f(): Argument #1 ($a) could not be passed by reference
