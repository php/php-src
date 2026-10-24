--TEST--
JSON_ALLOW_COMMENTS rejects malformed or misplaced comments
--FILE--
<?php

function decode_error(string $json, int $flags = JSON_ALLOW_COMMENTS): void {
    var_dump(json_decode($json, true, 512, $flags));
    echo json_last_error(), ": ", json_last_error_msg(), "\n";
}

/* Strict parsing is unchanged. */
decode_error("// comment\n1", 0);
decode_error('/* comment */ 1', 0);

/* Comments are trivia, not values or token fragments. */
decode_error('// comment only');
decode_error('/* comment only */');
decode_error("# comment\n1");
decode_error('/');
decode_error('tr/* comment */ue');
decode_error('1e/* comment */2');
decode_error('1./**/5');
decode_error('-/* comment */1');
decode_error('{"a" /* missing colon */ 1}');

/* Block comments do not nest; the first closing delimiter wins. */
decode_error('/* outer /* inner */ */ 1');

/* Unterminated block comments report their opening delimiter. */
decode_error('/* unterminated');
decode_error('/*/1');
decode_error("{\n  \"a\": 1,\n  /* unterminated\n  still open");

/* An embedded NUL is a control character error, also inside comments. */
foreach ([
    "[1, // a\x00b\n 2]",
    "[1, /* a\x00b */ 2]",
    "[1, /* x\n a\x00b */ 2]",
    "[1, /* a\x00b",
    "[1, // a\x00",
] as $json) {
    decode_error($json);
    var_dump(json_validate($json, 512, JSON_ALLOW_COMMENTS));
    echo json_last_error(), ": ", json_last_error_msg(), "\n";
}
decode_error("[1, // a\x00b\n 2]", JSON_ALLOW_COMMENTS | JSON_INVALID_UTF8_IGNORE);
decode_error("[1, /* a\x00b */ 2]", JSON_ALLOW_COMMENTS | JSON_INVALID_UTF8_SUBSTITUTE);

try {
    json_decode("[1,\n /* unterminated", true, 512,
        JSON_ALLOW_COMMENTS | JSON_THROW_ON_ERROR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getCode(), ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:16
NULL
4: Syntax error near location 1:19
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:2
NULL
4: Syntax error near location 1:2
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:26
NULL
4: Syntax error near location 1:22
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 3:3
NULL
3: Control character error, possibly incorrectly encoded near location 1:9
bool(false)
3: Control character error, possibly incorrectly encoded near location 1:9
NULL
3: Control character error, possibly incorrectly encoded near location 1:9
bool(false)
3: Control character error, possibly incorrectly encoded near location 1:9
NULL
3: Control character error, possibly incorrectly encoded near location 2:3
bool(false)
3: Control character error, possibly incorrectly encoded near location 2:3
NULL
3: Control character error, possibly incorrectly encoded near location 1:9
bool(false)
3: Control character error, possibly incorrectly encoded near location 1:9
NULL
3: Control character error, possibly incorrectly encoded near location 1:9
bool(false)
3: Control character error, possibly incorrectly encoded near location 1:9
NULL
3: Control character error, possibly incorrectly encoded near location 1:9
NULL
3: Control character error, possibly incorrectly encoded near location 1:9
JsonException: 4: Syntax error near location 2:2
