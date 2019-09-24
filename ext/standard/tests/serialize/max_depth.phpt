--TEST--
Bug #78549: Stack overflow due to nested serialized input
--FILE--
<?php

function create_nested_data($depth, $prefix, $suffix) {
    return str_repeat($prefix, $depth) . 'i:0;' . str_repeat($suffix, $depth);
}

var_dump(unserialize('i:0;', ['max_depth' => 'foo']));
var_dump(unserialize('i:0;', ['max_depth' => -1]));

var_dump(unserialize(
    create_nested_data(128, 'a:1:{i:0;', '}'),
    ['max_depth' => 128]
) !== false);
var_dump(unserialize(
    create_nested_data(129, 'a:1:{i:0;', '}'),
    ['max_depth' => 128]
));

var_dump(unserialize(
    create_nested_data(128, 'O:8:"stdClass":1:{i:0;', '}'),
    ['max_depth' => 128]
) !== false);
var_dump(unserialize(
    create_nested_data(129, 'O:8:"stdClass":1:{i:0;', '}'),
    ['max_depth' => 128]
));

// Default depth is 4096
var_dump(unserialize(create_nested_data(4096, 'a:1:{i:0;', '}')) !== false);
var_dump(unserialize(create_nested_data(4097, 'a:1:{i:0;', '}')));

?>
--EXPECTF--
Warning: unserialize(): max_depth should be int in %s on line %d
bool(false)

Warning: unserialize(): max_depth cannot be negative in %s on line %d
bool(false)
bool(true)

Warning: unserialize(): Maximum depth of 128 exceeded. The depth limit can be changed using the max_depth option in %s on line %d

Notice: unserialize(): Error at offset 1157 of 1294 bytes in %s on line %d
bool(false)
bool(true)

Warning: unserialize(): Maximum depth of 128 exceeded. The depth limit can be changed using the max_depth option in %s on line %d

Notice: unserialize(): Error at offset 2834 of 2971 bytes in %s on line %d
bool(false)
bool(true)

Warning: unserialize(): Maximum depth of 4096 exceeded. The depth limit can be changed using the max_depth option in %s on line %d

Notice: unserialize(): Error at offset 36869 of 40974 bytes in %s on line %d
bool(false)
