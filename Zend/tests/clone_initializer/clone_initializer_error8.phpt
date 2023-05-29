--TEST--
Test "clone with" with invalid property names
--FILE--
<?php

try {
    clone new stdClass() with [$undefined => 1];
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    clone new stdClass() with [$undefined => 1]; // The same as above but now using cache slots
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    clone new stdClass() with [null => 1];
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    clone new stdClass() with [null => 1]; // The same as above but now using cache slots
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    clone new stdClass() with [[] => 1];
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    clone new stdClass() with [[] => 1]; // The same as above but now using cache slots
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    clone new stdClass() with [1 => 1];
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    clone new stdClass() with [1 => 1]; // The same as above but now using cache slots
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
Warning: Undefined variable $undefined in %s on line %d
Property name must be of type string, null given

Warning: Undefined variable $undefined in %s on line %d
Property name must be of type string, null given
Property name must be of type string, null given
Property name must be of type string, null given
Property name must be of type string, array given
Property name must be of type string, array given
Property name must be of type string, int given
Property name must be of type string, int given
