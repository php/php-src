--TEST--
numfmt_set_pattern() with invalid pattern
--EXTENSIONS--
intl
--FILE--
<?php

$fmt = new NumberFormatter("en_US", NumberFormatter::PATTERN_DECIMAL);
$pattern = '0.0     .#.#.#';

var_dump($fmt->setPattern($pattern));
var_dump($fmt->getErrorMessage());
var_dump(numfmt_set_pattern($fmt, $pattern));
var_dump(numfmt_get_error_message($fmt));

?>
--EXPECT--
bool(false)
string(67) "Error setting pattern value at line 0, offset 0: U_UNQUOTED_SPECIAL"
bool(false)
string(67) "Error setting pattern value at line 0, offset 0: U_UNQUOTED_SPECIAL"
