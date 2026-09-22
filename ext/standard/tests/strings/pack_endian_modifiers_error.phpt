--TEST--
pack()/unpack() endianness modifiers, invalid combinations
--FILE--
<?php

$inherent_formats = ['n<', 'v>', 'N<', 'V>', 'J<', 'P>'];
foreach ($inherent_formats as $fmt) {
    try {
        pack($fmt, 1);
        echo "FAIL: Expected ValueError for pack('$fmt', 1)\n";
    } catch (ValueError $e) {
        echo "pack('$fmt'): " . $e->getMessage() . "\n";
    }
}

$inherent_float_formats = ['g<', 'G>', 'e<', 'E>'];
foreach ($inherent_float_formats as $fmt) {
    try {
        pack($fmt, 1.0);
        echo "FAIL: Expected ValueError for pack('$fmt', 1.0)\n";
    } catch (ValueError $e) {
        echo "pack('$fmt'): " . $e->getMessage() . "\n";
    }
}

$unsupported_formats = ['c<', 'C>', 'a<', 'A>', 'h<', 'H>', 'i<', 'I>', 'x<', 'X>', '@<'];
foreach ($unsupported_formats as $fmt) {
    try {
        pack($fmt, 1);
        echo "FAIL: Expected ValueError for pack('$fmt', 1)\n";
    } catch (ValueError $e) {
        echo "pack('$fmt'): " . $e->getMessage() . "\n";
    }
}

foreach (['n<', 'v>', 'N<', 'V>', 'J<', 'P>'] as $fmt) {
    try {
        unpack($fmt, "\x00\x00\x00\x00\x00\x00\x00\x00");
        echo "FAIL: Expected ValueError for unpack('$fmt', ...)\n";
    } catch (ValueError $e) {
        echo "unpack('$fmt'): " . $e->getMessage() . "\n";
    }
}

foreach (['g<', 'G>', 'e<', 'E>'] as $fmt) {
    try {
        unpack($fmt, "\x00\x00\x00\x00\x00\x00\x00\x00");
        echo "FAIL: Expected ValueError for unpack('$fmt', ...)\n";
    } catch (ValueError $e) {
        echo "unpack('$fmt'): " . $e->getMessage() . "\n";
    }
}

foreach (['c<', 'C>', 'a<', 'A>', 'h<', 'H>', 'i<', 'I>', 'x<', 'X>', '@<'] as $fmt) {
    try {
        unpack($fmt, "\x00\x00\x00\x00\x00\x00\x00\x00");
        echo "FAIL: Expected ValueError for unpack('$fmt', ...)\n";
    } catch (ValueError $e) {
        echo "unpack('$fmt'): " . $e->getMessage() . "\n";
    }
}
?>
--EXPECT--
pack('n<'): Endianness modifier '<' cannot be applied to format code 'n' which already has inherent endianness
pack('v>'): Endianness modifier '>' cannot be applied to format code 'v' which already has inherent endianness
pack('N<'): Endianness modifier '<' cannot be applied to format code 'N' which already has inherent endianness
pack('V>'): Endianness modifier '>' cannot be applied to format code 'V' which already has inherent endianness
pack('J<'): Endianness modifier '<' cannot be applied to format code 'J' which already has inherent endianness
pack('P>'): Endianness modifier '>' cannot be applied to format code 'P' which already has inherent endianness
pack('g<'): Endianness modifier '<' cannot be applied to format code 'g' which already has inherent endianness
pack('G>'): Endianness modifier '>' cannot be applied to format code 'G' which already has inherent endianness
pack('e<'): Endianness modifier '<' cannot be applied to format code 'e' which already has inherent endianness
pack('E>'): Endianness modifier '>' cannot be applied to format code 'E' which already has inherent endianness
pack('c<'): Endianness modifier is not supported for format code 'c'
pack('C>'): Endianness modifier is not supported for format code 'C'
pack('a<'): Endianness modifier is not supported for format code 'a'
pack('A>'): Endianness modifier is not supported for format code 'A'
pack('h<'): Endianness modifier is not supported for format code 'h'
pack('H>'): Endianness modifier is not supported for format code 'H'
pack('i<'): Endianness modifier is not supported for format code 'i'
pack('I>'): Endianness modifier is not supported for format code 'I'
pack('x<'): Endianness modifier is not supported for format code 'x'
pack('X>'): Endianness modifier is not supported for format code 'X'
pack('@<'): Endianness modifier is not supported for format code '@'
unpack('n<'): Endianness modifier '<' cannot be applied to format code 'n' which already has inherent endianness
unpack('v>'): Endianness modifier '>' cannot be applied to format code 'v' which already has inherent endianness
unpack('N<'): Endianness modifier '<' cannot be applied to format code 'N' which already has inherent endianness
unpack('V>'): Endianness modifier '>' cannot be applied to format code 'V' which already has inherent endianness
unpack('J<'): Endianness modifier '<' cannot be applied to format code 'J' which already has inherent endianness
unpack('P>'): Endianness modifier '>' cannot be applied to format code 'P' which already has inherent endianness
unpack('g<'): Endianness modifier '<' cannot be applied to format code 'g' which already has inherent endianness
unpack('G>'): Endianness modifier '>' cannot be applied to format code 'G' which already has inherent endianness
unpack('e<'): Endianness modifier '<' cannot be applied to format code 'e' which already has inherent endianness
unpack('E>'): Endianness modifier '>' cannot be applied to format code 'E' which already has inherent endianness
unpack('c<'): Endianness modifier is not supported for format code 'c'
unpack('C>'): Endianness modifier is not supported for format code 'C'
unpack('a<'): Endianness modifier is not supported for format code 'a'
unpack('A>'): Endianness modifier is not supported for format code 'A'
unpack('h<'): Endianness modifier is not supported for format code 'h'
unpack('H>'): Endianness modifier is not supported for format code 'H'
unpack('i<'): Endianness modifier is not supported for format code 'i'
unpack('I>'): Endianness modifier is not supported for format code 'I'
unpack('x<'): Endianness modifier is not supported for format code 'x'
unpack('X>'): Endianness modifier is not supported for format code 'X'
unpack('@<'): Endianness modifier is not supported for format code '@'
