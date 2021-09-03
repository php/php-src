--TEST--
Calling mb_convert_case() with an invalid casing mode
--EXTENSIONS--
mbstring
--FILE--
<?php

var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_UPPER));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_LOWER));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_TITLE));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_FOLD));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_UPPER_SIMPLE));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_LOWER_SIMPLE));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_TITLE_SIMPLE));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_FOLD_SIMPLE));

// Invalid mode
try {
    var_dump(mb_convert_case('foo BAR Spaß', 100));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
string(13) "FOO BAR SPASS"
string(13) "foo bar spaß"
string(13) "Foo Bar Spaß"
string(13) "foo bar spass"
string(13) "FOO BAR SPAß"
string(13) "foo bar spaß"
string(13) "Foo Bar Spaß"
string(13) "foo bar spaß"
mb_convert_case(): Argument #2 ($mode) must be one of the MB_CASE_* constants
