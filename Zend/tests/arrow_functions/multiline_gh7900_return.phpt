--TEST--
GH-7900: Arrow function with never return type compile-time errors (multiline, no errors)
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

$x = fn(): never => { throw new \Exception('Here'); };

try {
    var_dump($x());
} catch (\Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Here
