--TEST--
Bug GH-12745 (http_build_query() default null argument for $arg_separator is implicitly coerced to string)
--FILE--
<?php

$data = [
    'hello' => 'world',
    'foo' => 'bar',
];

var_dump(http_build_query(
    $data,
    encoding_type: PHP_QUERY_RFC3986
));
?>
--EXPECT--
string(19) "hello=world&foo=bar"
