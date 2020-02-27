--TEST--
Bug #79241: Segmentation fault on preg_match()
--FILE--
<?php

// if "’" string is used directly without json_decode,
// the issue does not reproduce
$text = json_decode('"’"');

$pattern = '/\b/u';

// it has to be exact two calls to preg_match(),
// with the second call offsetting after the tick symbol
var_dump(preg_match($pattern, $text, $matches, 0, 0));
var_dump(preg_match($pattern, $text, $matches, 0, 1));
var_dump(preg_last_error() == PREG_BAD_UTF8_OFFSET_ERROR);

echo "\n";

$text = "VA\xff"; $text .= "LID";
var_dump(preg_match($pattern, $text, $matches, 0, 4));
var_dump(preg_match($pattern, $text, $matches, 0, 0));
var_dump(preg_last_error() == PREG_BAD_UTF8_ERROR);

?>
--EXPECT--
int(0)
bool(false)
bool(true)

int(1)
bool(false)
bool(true)
