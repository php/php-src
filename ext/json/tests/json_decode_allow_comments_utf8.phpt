--TEST--
JSON_ALLOW_COMMENTS interactions with malformed UTF-8 flags
--FILE--
<?php

function show_decode(string $json, int $flags): void {
    var_dump(json_decode($json, true, 512, JSON_ALLOW_COMMENTS | $flags));
    echo json_last_error(), ': ', json_last_error_msg(), "\n";
}

function show_validate(string $json, int $flags): void {
    var_dump(json_validate($json, 512, JSON_ALLOW_COMMENTS | $flags));
    echo json_last_error(), ': ', json_last_error_msg(), "\n";
}

/* The invalid byte is only in the comment, so each result comes from the comment. */
$badBlockComment = "/* bad: \x80 */ [\"ab\"]";
$badLineComment = "// bad: \x80\n[\"ab\"]";

foreach ([$badBlockComment, $badLineComment] as $json) {
    show_decode($json, 0);
    show_decode($json, JSON_INVALID_UTF8_IGNORE);
    show_decode($json, JSON_INVALID_UTF8_SUBSTITUTE);
    show_validate($json, 0);
    show_validate($json, JSON_INVALID_UTF8_IGNORE);
}

try {
    json_decode($badLineComment, flags: JSON_ALLOW_COMMENTS | JSON_THROW_ON_ERROR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getCode(), ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
NULL
5: Malformed UTF-8 characters, possibly incorrectly encoded near location 1:9
array(1) {
  [0]=>
  string(2) "ab"
}
0: No error
array(1) {
  [0]=>
  string(2) "ab"
}
0: No error
bool(false)
5: Malformed UTF-8 characters, possibly incorrectly encoded near location 1:9
bool(true)
0: No error
NULL
5: Malformed UTF-8 characters, possibly incorrectly encoded near location 1:9
array(1) {
  [0]=>
  string(2) "ab"
}
0: No error
array(1) {
  [0]=>
  string(2) "ab"
}
0: No error
bool(false)
5: Malformed UTF-8 characters, possibly incorrectly encoded near location 1:9
bool(true)
0: No error
JsonException: 5: Malformed UTF-8 characters, possibly incorrectly encoded near location 1:9
