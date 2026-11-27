--TEST--
GH-25037 (Function JIT: "ENTRY - live var" for a namespaced function call with a recursive self:: call argument)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=32M
opcache.jit=1205
--EXTENSIONS--
opcache
mbstring
--FILE--
<?php
namespace Reproduce;

class Entry
{
    public static function chr(int $ord, string $encoding = 'UTF-8'): string
    {
        if ($encoding === 'UCS-4BE') {
            return pack('N', $ord);
        }

        return mb_convert_encoding(self::chr($ord, 'UCS-4BE'), $encoding, 'UCS-4BE');
    }
}

var_dump(bin2hex(Entry::chr(0x1F35E)));
?>
--EXPECT--
string(8) "f09f8d9e"
