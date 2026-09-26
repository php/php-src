--TEST--
unpack() element names starting with an endianness modifier on a format code that rejects it
--FILE--
<?php

foreach (["C>name", "@>name", "n<name", "V>name"] as $fmt) {
    try {
        unpack($fmt, "\x01\x02\x03\x04\x05\x06\x07\x08");
        echo "FAIL: Expected ValueError for unpack('$fmt', ...)\n";
    } catch (Throwable $e) {
        echo "unpack('$fmt'): ", $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
unpack('C>name'): ValueError: Endianness modifier is not supported for format code 'C'
unpack('@>name'): ValueError: Endianness modifier is not supported for format code '@'
unpack('n<name'): ValueError: Endianness modifier '<' cannot be applied to format code 'n' which already has inherent endianness
unpack('V>name'): ValueError: Endianness modifier '>' cannot be applied to format code 'V' which already has inherent endianness
