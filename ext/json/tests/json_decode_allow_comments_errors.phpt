--TEST--
JSON_ALLOW_COMMENTS rejects malformed or misplaced comments
--FILE--
<?php

function decode_error(string $json, int $flags = JSON_ALLOW_COMMENTS): void {
    var_dump(json_decode($json, true, 512, $flags));
    echo json_last_error(), ": ", json_last_error_msg(), "\n";
}

/* Strict parsing is unchanged. */
decode_error('// comment\n1', 0);
decode_error('/* comment */ 1', 0);

/* Comments are trivia, not values or token fragments. */
decode_error('// comment only');
decode_error('/* comment only */');
decode_error('# comment\n1');
decode_error('/');
decode_error('tr/* comment */ue');
decode_error('1e/* comment */2');
decode_error('-/* comment */1');
decode_error('{"a" /* missing colon */ 1}');

/* Block comments do not nest; the first closing delimiter wins. */
decode_error('/* outer /* inner */ */ 1');

/* Unterminated block comments report their opening delimiter. */
decode_error('/* unterminated');
decode_error("{\n  \"a\": 1,\n  /* unterminated\n  still open");

try {
    json_decode("[1,\n /* unterminated", true, 512,
        JSON_ALLOW_COMMENTS | JSON_THROW_ON_ERROR);
} catch (JsonException $e) {
    echo get_class($e), ': ', $e->getCode(), ': ', $e->getMessage(), "\n";
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
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:26
NULL
4: Syntax error near location 1:22
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 3:3
JsonException: 4: Syntax error near location 2:2
