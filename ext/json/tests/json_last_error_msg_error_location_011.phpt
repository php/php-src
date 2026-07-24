--TEST--
json_last_error_msg() - Error location reporting with JSONC comments
--FILE--
<?php

require_once("json_validate_requires.inc");

// The newlines inside a block comment count towards the reported line.
echo "Error on line 4, column 20:\n";
$json = '{
/* line two
line three
line four */ "key" 1
}';
json_validate_trycatchdump($json, 512, JSON_ALLOW_COMMENTS);

echo "\nError on line 3, column 4:\n";
$json = '// one
// two
[1 2]';
json_validate_trycatchdump($json, 512, JSON_ALLOW_COMMENTS);

// An inline block comment occupies its raw columns.
echo "\nError on line 1, column 12:\n";
json_validate_trycatchdump("[1 /* c */ 2]", 512, JSON_ALLOW_COMMENTS);

// Columns count characters, not bytes, including inside comments.
echo "\nError on line 1, column 22:\n";
json_validate_trycatchdump("[1 /* héllo wörld */ 2]", 512, JSON_ALLOW_COMMENTS);

// An unterminated block comment is reported at its opener.
echo "\nError on line 3, column 3:\n";
$json = '{
  "a": 1,
  /* never closed
  "b": 2
}';
json_validate_trycatchdump($json, 512, JSON_ALLOW_COMMENTS);

// A comment between a trailing comma and a stray comma keeps locations exact.
echo "\nError on line 1, column 13:\n";
json_validate_trycatchdump("[1, /* x */ ,]", 512, JSON_ALLOW_COMMENTS | JSON_ALLOW_TRAILING_COMMAS);

?>
--EXPECT--
Error on line 4, column 20:
bool(false)
int(4)
string(31) "Syntax error near location 4:20"

Error on line 3, column 4:
bool(false)
int(4)
string(30) "Syntax error near location 3:4"

Error on line 1, column 12:
bool(false)
int(4)
string(31) "Syntax error near location 1:12"

Error on line 1, column 22:
bool(false)
int(4)
string(31) "Syntax error near location 1:22"

Error on line 3, column 3:
bool(false)
int(4)
string(30) "Syntax error near location 3:3"

Error on line 1, column 13:
bool(false)
int(4)
string(31) "Syntax error near location 1:13"
