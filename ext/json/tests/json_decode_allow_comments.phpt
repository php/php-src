--TEST--
json_decode() and json_validate() with JSON_ALLOW_COMMENTS
--FILE--
<?php

function decode_and_validate(string $json): void {
    $value = json_decode($json, true, 512, JSON_ALLOW_COMMENTS);
    echo json_encode($value, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES), "\n";
    var_dump(json_validate($json, 512, JSON_ALLOW_COMMENTS));
}

/* Exercise every whitespace position in objects and arrays. */
decode_and_validate('/* before root */ {
    /* after object opener */
    "a" /* after key */ : /* after colon */
    [ /* after array opener */ 1 /* before comma */,
      /* after comma */ 2 /* before array closer */ ]
    /* after value */, /* after member comma */
    "b"/* before colon */:/* after colon */3/* before object closer */
} /* after root */');

decode_and_validate('[1 // before comma
, 2 // before closer
]');
decode_and_validate('/* object */{/* only trivia */}');
decode_and_validate('/* array */[/* only trivia */]');

/* Line comments end at LF, CR, CRLF, or end-of-input. */
decode_and_validate("// LF\n1");
decode_and_validate("// CR\r2");
decode_and_validate("// CRLF\r\n3");
decode_and_validate("4// EOF");
decode_and_validate("// U+2028 \u{2028} and U+2029 \u{2029}\n6");
decode_and_validate("// embedded NUL: a\x00b\n7");

/* Comment delimiters inside strings are ordinary string content. */
decode_and_validate('["//", "/* not a comment */", "https://php.net/a/*b*/"]');

/* Valid Unicode, Unicode line separators, controls, and NUL are comment text. */
decode_and_validate("/* héllo \u{2028} world \u{2029} \x01\x0c\x00 */ 5");
decode_and_validate('/* ' . str_repeat('x', 65536) . ' */ 8');

?>
--EXPECT--
{"a":[1,2],"b":3}
bool(true)
[1,2]
bool(true)
[]
bool(true)
[]
bool(true)
1
bool(true)
2
bool(true)
3
bool(true)
4
bool(true)
6
bool(true)
7
bool(true)
["//","/* not a comment */","https://php.net/a/*b*/"]
bool(true)
5
bool(true)
8
bool(true)
