--TEST--
SCCP 023: ADD_ARRAY_ELEMENT with partial array
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0
--EXTENSIONS--
opcache
--FILE--
<?php
function a ($field_type, $allowed_values) {
    $settings = [
        'list_string' => [
            'allowed_values' => $allowed_values,
        ],
    ];

    return $settings[$field_type];
}

var_dump(a("list_string", ["xxx"]));
?>
--EXPECT--
array(1) {
  ["allowed_values"]=>
  array(1) {
    [0]=>
    string(3) "xxx"
  }
}
