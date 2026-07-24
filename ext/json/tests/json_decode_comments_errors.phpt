--TEST--
json_decode() with JSON_ALLOW_COMMENTS - malformed comments and other errors
--FILE--
<?php

function decode_dump(string $json, int $flags = JSON_ALLOW_COMMENTS) {
    var_dump(json_decode($json, true, 512, $flags));
    echo json_last_error(), ": ", json_last_error_msg(), "\n";
}

// unterminated block comments: error at the comment opener
decode_dump("/*");
decode_dump("/* text");
decode_dump("{\n /* x\n y");
// lone slashes are still syntax errors
decode_dump("/");
decode_dump("/ /");
// the first */ closes the comment, the stray one is an error
decode_dump("/* /* */ */1");
// hash comments are not supported
decode_dump("# comment\n1");
// comment-only documents behave like whitespace-only documents
decode_dump(" ", 0);
decode_dump("//x");
decode_dump("/*x*/");
decode_dump("// x\n");
// comments separate tokens but do not join values
decode_dump("1 2", 0);
decode_dump("1/*c*/2");
// a comment does not replace a required colon
decode_dump("{\"a\" /*c*/ 1}");

try {
    json_decode("/*", true, 512, JSON_ALLOW_COMMENTS | JSON_THROW_ON_ERROR);
} catch (JsonException $e) {
    echo "JsonException: ", $e->getCode(), " ", $e->getMessage(), "\n";
}

?>
--EXPECT--
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 2:2
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:10
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:2
NULL
4: Syntax error near location 1:4
NULL
4: Syntax error near location 1:6
NULL
4: Syntax error near location 2:1
NULL
4: Syntax error near location 1:3
NULL
4: Syntax error near location 1:7
NULL
4: Syntax error near location 1:12
JsonException: 4 Syntax error near location 1:1
