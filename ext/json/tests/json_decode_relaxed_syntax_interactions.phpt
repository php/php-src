--TEST--
JSON comment and trailing-comma flags interact correctly with existing behavior
--FILE--
<?php

const RELAXED = JSON_ALLOW_COMMENTS | JSON_ALLOW_TRAILING_COMMAS;

var_dump(JSON_ALLOW_COMMENTS, JSON_ALLOW_TRAILING_COMMAS);
var_dump(json_encode(['x' => 1], RELAXED));

/* Association, bigint conversion, and duplicate-key behavior are unchanged. */
var_dump(json_decode(
    '/* before */{"n":9223372036854775808,"same":1,"same":2,}',
    null,
    512,
    RELAXED | JSON_OBJECT_AS_ARRAY | JSON_BIGINT_AS_STRING
));
var_dump(json_decode('{"a":1,}', false, 512, JSON_ALLOW_TRAILING_COMMAS));

/* Comments and trailing commas do not consume nesting depth. */
$nested = '/* before */[[0,],]';
var_dump(json_decode($nested, true, 2, RELAXED));
echo json_last_error(), ': ', json_last_error_msg(), "\n";
var_dump(json_validate($nested, 3, RELAXED));

/* A trailing-comma reduction must not replace a more specific scanner error. */
foreach ([
    "[1,\x01]",
    "[1,\x80]",
    '[1,"\\ud834"]',
] as $json) {
    var_dump(json_decode($json, true, 512, RELAXED));
    echo json_last_error(), ': ', json_last_error_msg(), "\n";
}

/* json_validate() accepts only its documented combinations. */
var_dump(json_validate('/* comment */[1,]', 512, RELAXED));
var_dump(json_validate(
    "/* \x80 */[1,]",
    512,
    RELAXED | JSON_INVALID_UTF8_IGNORE
));
foreach ([JSON_BIGINT_AS_STRING, JSON_INVALID_UTF8_SUBSTITUTE] as $invalidFlag) {
    try {
        json_validate('{}', flags: RELAXED | $invalidFlag);
    } catch (ValueError $e) {
        echo get_class($e), "\n";
    }
}

?>
--EXPECTF--
int(8388608)
int(16777216)
string(7) "{"x":1}"
array(2) {
  ["n"]=>
  string(19) "9223372036854775808"
  ["same"]=>
  int(2)
}
object(stdClass)#%d (1) {
  ["a"]=>
  int(1)
}
NULL
1: Maximum stack depth exceeded near location 1:14
bool(true)
NULL
3: Control character error, possibly incorrectly encoded near location 1:4
NULL
5: Malformed UTF-8 characters, possibly incorrectly encoded near location 1:4
NULL
10: Single unpaired UTF-16 surrogate in unicode escape near location 1:4
bool(true)
bool(true)
ValueError
ValueError
