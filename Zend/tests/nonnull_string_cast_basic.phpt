--TEST--
Non-null string cast: basic functionality
--FILE--
<?php

var_dump((!string) "hello");
var_dump((!string) 123);
var_dump((!string) 45.67);
var_dump((!string) true);
var_dump((!string) false);

class StringableObject {
    public function __toString(): string {
        return "stringable";
    }
}

class NonStringableObject {}

$stringable = new StringableObject();
$nonStringable = new NonStringableObject();

var_dump((!string) $stringable);

try {
    var_dump((!string) $nonStringable);
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
string(5) "hello"
string(3) "123"
string(5) "45.67"
string(1) "1"
string(0) ""
string(10) "stringable"
TypeError: Cannot cast NonStringableObject to string
