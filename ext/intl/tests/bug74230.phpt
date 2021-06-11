--TEST--
Bug #74230 iconv fails to fail on surrogates
--EXTENSIONS--
iconv
--FILE--
<?php

$high = "\xED\xa1\x92"; // codepoint D852
$low = "\xED\xBD\xA2"; // codepoint DF62
$pair = $high.$low;
var_dump(
    @\iconv('UTF-8', 'UTF-8', $high) === false,
    @\iconv('UTF-8', 'UTF-8', $low) === false,
    @\iconv('UTF-8', 'UTF-8', $pair) === false
);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
