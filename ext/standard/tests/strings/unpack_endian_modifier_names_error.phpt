--TEST--
unpack() element names starting with an endianness modifier on a format code that rejects it
--FILE--
<?php

foreach (["C>name", "@>name", "n<name", "V>name"] as $fmt) {
    try {
        unpack($fmt, "\x01\x02\x03\x04\x05\x06\x07\x08");
        echo "FAIL: Expected ValueError for unpack('$fmt', ...)\n";
    } catch (ValueError $e) {
        echo "unpack('$fmt'): " . $e->getMessage() . "\n";
    }
}

?>
--EXPECT--
unpack('C>name'): Endianness modifier is not supported for format code 'C'
unpack('@>name'): Endianness modifier is not supported for format code '@'
unpack('n<name'): Endianness modifier '<' cannot be applied to format code 'n' which already has inherent endianness
unpack('V>name'): Endianness modifier '>' cannot be applied to format code 'V' which already has inherent endianness
