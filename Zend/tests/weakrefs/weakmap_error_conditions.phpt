--TEST--
WeakMap error conditions
--FILE--
<?php

$map = new WeakMap;
try {
    $map[1] = 2;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($map[1]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    isset($map[1]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($map[1]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $map[] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $map[][1] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($map[new stdClass]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    $map->prop = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($map->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $r =& $map->prop;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    isset($map->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($map->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    serialize($map);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
try {
    unserialize('C:7:"WeakMap":0:{}');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
WeakMap key must be an object
WeakMap key must be an object
WeakMap key must be an object
WeakMap key must be an object
Cannot append to WeakMap
Cannot append to WeakMap
Object stdClass#2 not contained in WeakMap
WeakMap objects do not support properties
WeakMap objects do not support properties
WeakMap objects do not support property references
WeakMap objects do not support properties
WeakMap objects do not support properties
Serialization of 'WeakMap' is not allowed
Unserialization of 'WeakMap' is not allowed
