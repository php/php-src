--TEST--
JSON_ALLOW_COMMENTS interactions with malformed UTF-8 flags
--FILE--
<?php

function show_decode(string $json, int $flags): void {
    var_dump(json_decode($json, true, 512, JSON_ALLOW_COMMENTS | $flags));
    echo json_last_error(), "\n";
}

$badBlockComment = "/* bad: \x80 */ [\"a\x80b\"]";
$badLineComment = "// bad: \x80\n1";

show_decode($badBlockComment, 0);
show_decode($badBlockComment, JSON_INVALID_UTF8_IGNORE);
show_decode($badBlockComment, JSON_INVALID_UTF8_SUBSTITUTE);
show_decode($badLineComment, 0);
show_decode($badLineComment, JSON_INVALID_UTF8_IGNORE);

var_dump(json_validate($badBlockComment, 512, JSON_ALLOW_COMMENTS));
echo json_last_error(), "\n";
var_dump(json_validate(
    $badBlockComment,
    512,
    JSON_ALLOW_COMMENTS | JSON_INVALID_UTF8_IGNORE
));
echo json_last_error(), "\n";

try {
    json_decode($badLineComment, flags: JSON_ALLOW_COMMENTS | JSON_THROW_ON_ERROR);
} catch (JsonException $e) {
    echo $e->getCode(), ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
NULL
5
array(1) {
  [0]=>
  string(2) "ab"
}
0
array(1) {
  [0]=>
  string(5) "a�b"
}
0
NULL
5
int(1)
0
bool(false)
5
bool(true)
0
5: Malformed UTF-8 characters, possibly incorrectly encoded near location 1:9
