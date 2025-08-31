--TEST--
Bug #78549: Stack overflow due to nested serialized input
--SKIPIF--
<?php
if (getenv('SKIP_PRELOAD')) die('skip Different order of deprecation messages');
?>
--FILE--
<?php

function create_nested_data($depth, $prefix, $suffix, $inner = 'i:0;') {
    return str_repeat($prefix, $depth) . $inner . str_repeat($suffix, $depth);
}

echo "Invalid max_depth:\n";
try {
    unserialize('i:0;', ['max_depth' => 'foo']);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    unserialize('i:0;', ['max_depth' => -1]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Array:\n";
var_dump(unserialize(
    create_nested_data(128, 'a:1:{i:0;', '}'),
    ['max_depth' => 128]
) !== false);
var_dump(unserialize(
    create_nested_data(129, 'a:1:{i:0;', '}'),
    ['max_depth' => 128]
));

echo "Object:\n";
var_dump(unserialize(
    create_nested_data(128, 'O:8:"stdClass":1:{i:0;', '}'),
    ['max_depth' => 128]
) !== false);
var_dump(unserialize(
    create_nested_data(129, 'O:8:"stdClass":1:{i:0;', '}'),
    ['max_depth' => 128]
));

// Depth can also be adjusted using ini setting
echo "Ini setting:\n";
ini_set("unserialize_max_depth", 128);
var_dump(unserialize(create_nested_data(128, 'a:1:{i:0;', '}')) !== false);
var_dump(unserialize(create_nested_data(129, 'a:1:{i:0;', '}')));

// But an explicitly specified depth still takes precedence
echo "Ini setting overridden:\n";
var_dump(unserialize(
    create_nested_data(256, 'a:1:{i:0;', '}'),
    ['max_depth' => 256]
) !== false);
var_dump(unserialize(
    create_nested_data(257, 'a:1:{i:0;', '}'),
    ['max_depth' => 256]
));

// Reset ini setting to a large value,
// so it's clear that it won't be used in the following.
ini_set("unserialize_max_depth", 4096);

class Test implements Serializable {
    public function serialize() {
        return '';
    }
    public function unserialize($str) {
        // Should fail, due to combined nesting level
        var_dump(unserialize(create_nested_data(129, 'a:1:{i:0;', '}')));
        // Should succeed, below combined nesting level
        var_dump(unserialize(create_nested_data(128, 'a:1:{i:0;', '}')) !== false);
    }
}
echo "Nested unserialize combined depth limit:\n";
var_dump(is_array(unserialize(
    create_nested_data(128, 'a:1:{i:0;', '}', 'C:4:"Test":0:{}'),
    ['max_depth' => 256]
)));

class Test2 implements Serializable {
    public function serialize() {
        return '';
    }
    public function unserialize($str) {
        // If depth limit is overridden, the depth should be counted
        // from zero again.
        var_dump(unserialize(
            create_nested_data(257, 'a:1:{i:0;', '}'),
            ['max_depth' => 256]
        ));
        var_dump(unserialize(
            create_nested_data(256, 'a:1:{i:0;', '}'),
            ['max_depth' => 256]
        ) !== false);
    }
}
echo "Nested unserialize overridden depth limit:\n";
var_dump(is_array(unserialize(
    create_nested_data(64, 'a:1:{i:0;', '}', 'C:5:"Test2":0:{}'),
    ['max_depth' => 128]
)));

?>
--EXPECTF--
Invalid max_depth:
unserialize(): Option "max_depth" must be of type int, string given
unserialize(): Option "max_depth" must be greater than or equal to 0
Array:
bool(true)

Warning: unserialize(): Maximum depth of 128 exceeded. The depth limit can be changed using the max_depth unserialize() option or the unserialize_max_depth ini setting in %s on line %d

Warning: unserialize(): Error at offset 1157 of 1294 bytes in %s on line %d
bool(false)
Object:
bool(true)

Warning: unserialize(): Maximum depth of 128 exceeded. The depth limit can be changed using the max_depth unserialize() option or the unserialize_max_depth ini setting in %s on line %d

Warning: unserialize(): Error at offset 2834 of 2971 bytes in %s on line %d
bool(false)
Ini setting:
bool(true)

Warning: unserialize(): Maximum depth of 128 exceeded. The depth limit can be changed using the max_depth unserialize() option or the unserialize_max_depth ini setting in %s on line %d

Warning: unserialize(): Error at offset 1157 of 1294 bytes in %s on line %d
bool(false)
Ini setting overridden:
bool(true)

Warning: unserialize(): Maximum depth of 256 exceeded. The depth limit can be changed using the max_depth unserialize() option or the unserialize_max_depth ini setting in %s on line %d

Warning: unserialize(): Error at offset 2309 of 2574 bytes in %s on line %d
bool(false)

Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
Nested unserialize combined depth limit:

Warning: unserialize(): Maximum depth of 256 exceeded. The depth limit can be changed using the max_depth unserialize() option or the unserialize_max_depth ini setting in %s on line %d

Warning: unserialize(): Error at offset 1157 of 1294 bytes in %s on line %d
bool(false)
bool(true)
bool(true)

Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
Nested unserialize overridden depth limit:

Warning: unserialize(): Maximum depth of 256 exceeded. The depth limit can be changed using the max_depth unserialize() option or the unserialize_max_depth ini setting in %s on line %d

Warning: unserialize(): Error at offset 2309 of 2574 bytes in %s on line %d
bool(false)
bool(true)
bool(true)
