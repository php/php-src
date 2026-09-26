--TEST--
pack()/unpack() endianness modifiers, invalid combinations
--FILE--
<?php

$inherent_formats = ['n<', 'v>', 'N<', 'V>', 'J<', 'P>'];
foreach ($inherent_formats as $fmt) {
    try {
        pack($fmt, 1);
        echo "FAIL: Expected ValueError for pack('$fmt', 1)\n";
    } catch (Throwable $e) {
        echo "pack('$fmt'): ", $e::class, ': ', $e->getMessage(), "\n";
    }
}

$inherent_float_formats = ['g<', 'G>', 'e<', 'E>'];
foreach ($inherent_float_formats as $fmt) {
    try {
        pack($fmt, 1.0);
        echo "FAIL: Expected ValueError for pack('$fmt', 1.0)\n";
    } catch (Throwable $e) {
        echo "pack('$fmt'): ", $e::class, ': ', $e->getMessage(), "\n";
    }
}

$unsupported_formats = ['c<', 'C>', 'a<', 'A>', 'h<', 'H>', 'i<', 'I>', 'x<', 'X>', '@<'];
foreach ($unsupported_formats as $fmt) {
    try {
        pack($fmt, 1);
        echo "FAIL: Expected ValueError for pack('$fmt', 1)\n";
    } catch (Throwable $e) {
        echo "pack('$fmt'): ", $e::class, ': ', $e->getMessage(), "\n";
    }
}

foreach (['n<', 'v>', 'N<', 'V>', 'J<', 'P>'] as $fmt) {
    try {
        unpack($fmt, "\x00\x00\x00\x00\x00\x00\x00\x00");
        echo "FAIL: Expected ValueError for unpack('$fmt', ...)\n";
    } catch (Throwable $e) {
        echo "unpack('$fmt'): ", $e::class, ': ', $e->getMessage(), "\n";
    }
}

foreach (['g<', 'G>', 'e<', 'E>'] as $fmt) {
    try {
        unpack($fmt, "\x00\x00\x00\x00\x00\x00\x00\x00");
        echo "FAIL: Expected ValueError for unpack('$fmt', ...)\n";
    } catch (Throwable $e) {
        echo "unpack('$fmt'): ", $e::class, ': ', $e->getMessage(), "\n";
    }
}

foreach (['c<', 'C>', 'a<', 'A>', 'h<', 'H>', 'i<', 'I>', 'x<', 'X>', '@<'] as $fmt) {
    try {
        unpack($fmt, "\x00\x00\x00\x00\x00\x00\x00\x00");
        echo "FAIL: Expected ValueError for unpack('$fmt', ...)\n";
    } catch (Throwable $e) {
        echo "unpack('$fmt'): ", $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
pack('n<'): ValueError: Endianness modifier '<' cannot be applied to format code 'n' which already has inherent endianness
pack('v>'): ValueError: Endianness modifier '>' cannot be applied to format code 'v' which already has inherent endianness
pack('N<'): ValueError: Endianness modifier '<' cannot be applied to format code 'N' which already has inherent endianness
pack('V>'): ValueError: Endianness modifier '>' cannot be applied to format code 'V' which already has inherent endianness
pack('J<'): ValueError: Endianness modifier '<' cannot be applied to format code 'J' which already has inherent endianness
pack('P>'): ValueError: Endianness modifier '>' cannot be applied to format code 'P' which already has inherent endianness
pack('g<'): ValueError: Endianness modifier '<' cannot be applied to format code 'g' which already has inherent endianness
pack('G>'): ValueError: Endianness modifier '>' cannot be applied to format code 'G' which already has inherent endianness
pack('e<'): ValueError: Endianness modifier '<' cannot be applied to format code 'e' which already has inherent endianness
pack('E>'): ValueError: Endianness modifier '>' cannot be applied to format code 'E' which already has inherent endianness
pack('c<'): ValueError: Endianness modifier is not supported for format code 'c'
pack('C>'): ValueError: Endianness modifier is not supported for format code 'C'
pack('a<'): ValueError: Endianness modifier is not supported for format code 'a'
pack('A>'): ValueError: Endianness modifier is not supported for format code 'A'
pack('h<'): ValueError: Endianness modifier is not supported for format code 'h'
pack('H>'): ValueError: Endianness modifier is not supported for format code 'H'
pack('i<'): ValueError: Endianness modifier is not supported for format code 'i'
pack('I>'): ValueError: Endianness modifier is not supported for format code 'I'
pack('x<'): ValueError: Endianness modifier is not supported for format code 'x'
pack('X>'): ValueError: Endianness modifier is not supported for format code 'X'
pack('@<'): ValueError: Endianness modifier is not supported for format code '@'
unpack('n<'): ValueError: Endianness modifier '<' cannot be applied to format code 'n' which already has inherent endianness
unpack('v>'): ValueError: Endianness modifier '>' cannot be applied to format code 'v' which already has inherent endianness
unpack('N<'): ValueError: Endianness modifier '<' cannot be applied to format code 'N' which already has inherent endianness
unpack('V>'): ValueError: Endianness modifier '>' cannot be applied to format code 'V' which already has inherent endianness
unpack('J<'): ValueError: Endianness modifier '<' cannot be applied to format code 'J' which already has inherent endianness
unpack('P>'): ValueError: Endianness modifier '>' cannot be applied to format code 'P' which already has inherent endianness
unpack('g<'): ValueError: Endianness modifier '<' cannot be applied to format code 'g' which already has inherent endianness
unpack('G>'): ValueError: Endianness modifier '>' cannot be applied to format code 'G' which already has inherent endianness
unpack('e<'): ValueError: Endianness modifier '<' cannot be applied to format code 'e' which already has inherent endianness
unpack('E>'): ValueError: Endianness modifier '>' cannot be applied to format code 'E' which already has inherent endianness
unpack('c<'): ValueError: Endianness modifier is not supported for format code 'c'
unpack('C>'): ValueError: Endianness modifier is not supported for format code 'C'
unpack('a<'): ValueError: Endianness modifier is not supported for format code 'a'
unpack('A>'): ValueError: Endianness modifier is not supported for format code 'A'
unpack('h<'): ValueError: Endianness modifier is not supported for format code 'h'
unpack('H>'): ValueError: Endianness modifier is not supported for format code 'H'
unpack('i<'): ValueError: Endianness modifier is not supported for format code 'i'
unpack('I>'): ValueError: Endianness modifier is not supported for format code 'I'
unpack('x<'): ValueError: Endianness modifier is not supported for format code 'x'
unpack('X>'): ValueError: Endianness modifier is not supported for format code 'X'
unpack('@<'): ValueError: Endianness modifier is not supported for format code '@'
