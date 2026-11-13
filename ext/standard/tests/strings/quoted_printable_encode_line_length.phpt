--TEST--
quoted_printable_encode() keeps lines within 76 characters for bytes 0xF5 to 0xFF
--FILE--
<?php

foreach (["\xF5", "\xF8", "\xFF"] as $byte) {
    $str = str_repeat($byte, 30);
    $encoded = quoted_printable_encode($str);
    echo bin2hex($byte), ": ", implode(",", array_map('strlen', explode("\r\n", $encoded))), "\n";
    var_dump(quoted_printable_decode($encoded) === $str);
}

?>
--EXPECT--
f5: 76,15
bool(true)
f8: 76,15
bool(true)
ff: 76,15
bool(true)
