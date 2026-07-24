--TEST--
JSONC syntax is rejected when JSON_ALLOW_COMMENTS / JSON_ALLOW_TRAILING_COMMAS are not set
--FILE--
<?php

function decode_dump(string $json, int $flags = 0) {
    var_dump(json_decode($json, true, 512, $flags));
    echo json_last_error(), ": ", json_last_error_msg(), "\n";
}

// no flags: comments are syntax errors at the first slash
decode_dump("// x");
decode_dump("/* x */ 1");
decode_dump("/**/");
decode_dump("[1, 2] // t");
// no flags: trailing commas are syntax errors at the closer
decode_dump("[1,]");
decode_dump("{\"a\":1,}");
decode_dump("[1, 2,]");
// one flag does not enable the other relaxation
decode_dump("[1,]", JSON_ALLOW_COMMENTS);
decode_dump("[1, /*c*/ 2]", JSON_ALLOW_TRAILING_COMMAS);
// validate behaves identically
var_dump(json_validate("// x\n1", 512, JSON_ALLOW_TRAILING_COMMAS));
var_dump(json_validate("[1,]", 512, JSON_ALLOW_COMMENTS));

?>
--EXPECT--
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:1
NULL
4: Syntax error near location 1:8
NULL
4: Syntax error near location 1:4
NULL
4: Syntax error near location 1:8
NULL
4: Syntax error near location 1:7
NULL
4: Syntax error near location 1:4
NULL
4: Syntax error near location 1:5
bool(false)
bool(false)
