--TEST--
JSON comment and trailing-comma errors preserve original source locations
--FILE--
<?php

function validate_error(string $json, int $flags): void {
    var_dump(json_validate($json, 512, $flags));
    echo json_last_error(), ': ', json_last_error_msg(), "\n";
}

/* Newlines within skipped block comments still advance source locations. */
validate_error("{\n/* line 2\nline 3 */ \"key\" 1\n}", JSON_ALLOW_COMMENTS);

/* CR, LF, and CRLF each count as one line ending. */
validate_error("// CR\r// LF\n// CRLF\r\n[1 2]", JSON_ALLOW_COMMENTS);

/* Columns count Unicode characters, not UTF-8 bytes. */
validate_error('[1 /* héllo wörld */ 2]', JSON_ALLOW_COMMENTS);

/* Unterminated comments are located at their opening slash. */
validate_error("{\n  \"a\": 1,\n  /* open\n  still open", JSON_ALLOW_COMMENTS);

/* Comments do not hide the position of a repeated comma. */
validate_error(
    '[1, /* comment */ ,]',
    JSON_ALLOW_COMMENTS | JSON_ALLOW_TRAILING_COMMAS
);

?>
--EXPECT--
bool(false)
4: Syntax error near location 3:17
bool(false)
4: Syntax error near location 4:4
bool(false)
4: Syntax error near location 1:22
bool(false)
4: Syntax error near location 3:3
bool(false)
4: Syntax error near location 1:19
