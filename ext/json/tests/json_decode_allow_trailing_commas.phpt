--TEST--
json_decode() and json_validate() with JSON_ALLOW_TRAILING_COMMAS
--FILE--
<?php

function decode_result(string $json, int $flags = JSON_ALLOW_TRAILING_COMMAS): void {
    $value = json_decode($json, true, 512, $flags);
    if (json_last_error() === JSON_ERROR_NONE) {
        echo json_encode($value), "\n";
    } else {
        echo 'error ', json_last_error(), ': ', json_last_error_msg(), "\n";
    }
}

decode_result('[1,]');
decode_result('[1, ]');
decode_result("[1,\n]");
decode_result('{"a":1,}');
decode_result('[[1,],{"a":2,},]');

/* A trailing comma is valid only after a value in a non-empty container. */
decode_result('[,]');
decode_result('{,}');
decode_result('[1,,]');
decode_result('[1,,2]');
decode_result('{"a":1,,}');

/* Relaxations are independent and strict parsing is unchanged. */
decode_result('[1,]', 0);
decode_result('[1,]', JSON_ALLOW_COMMENTS);
decode_result('[1,/* comment */]', JSON_ALLOW_TRAILING_COMMAS);
decode_result(
    '[1,/* comment */]',
    JSON_ALLOW_COMMENTS | JSON_ALLOW_TRAILING_COMMAS
);
decode_result(
    "{\"a\":1,// comment\n}",
    JSON_ALLOW_COMMENTS | JSON_ALLOW_TRAILING_COMMAS
);

/* A trailing comma does not make a mismatched closer valid. */
decode_result('[1,}');
decode_result('{"a":1,]');

var_dump(json_validate('[1,]', 512, JSON_ALLOW_TRAILING_COMMAS));
var_dump(json_validate('{"a":1,}', 512, JSON_ALLOW_TRAILING_COMMAS));
var_dump(json_validate('[1,,]', 512, JSON_ALLOW_TRAILING_COMMAS));
var_dump(json_validate(
    '[1, /* comment */]',
    512,
    JSON_ALLOW_COMMENTS | JSON_ALLOW_TRAILING_COMMAS
));
var_dump(json_validate(
    '[/* comment */,]',
    512,
    JSON_ALLOW_COMMENTS | JSON_ALLOW_TRAILING_COMMAS
));
var_dump(json_validate(
    '{/* comment */,}',
    512,
    JSON_ALLOW_COMMENTS | JSON_ALLOW_TRAILING_COMMAS
));

try {
    json_decode('[1,,]', true, 512,
        JSON_ALLOW_TRAILING_COMMAS | JSON_THROW_ON_ERROR);
} catch (JsonException $e) {
    echo $e->getCode(), ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
[1]
[1]
[1]
{"a":1}
[[1],{"a":2}]
error 4: Syntax error near location 1:2
error 4: Syntax error near location 1:2
error 4: Syntax error near location 1:4
error 4: Syntax error near location 1:4
error 4: Syntax error near location 1:8
error 4: Syntax error near location 1:4
error 4: Syntax error near location 1:4
error 4: Syntax error near location 1:4
[1]
{"a":1}
error 2: State mismatch (invalid or malformed JSON) near location 1:4
error 2: State mismatch (invalid or malformed JSON) near location 1:8
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
4: Syntax error near location 1:4
