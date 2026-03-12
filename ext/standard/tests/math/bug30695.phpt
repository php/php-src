--TEST--
Bug #30695 (32 bit issues)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
    function toUTF8( $char_code )
    {
        switch ( $char_code )
        {
            case 0:
                $char = chr( 0 );
                break;
            case !($char_code & 0xffffff80): // 7 bit
                $char = chr( $char_code );
                break;
            case !($char_code & 0xfffff800): // 11 bit
                $char = ( chr(0xc0 | (($char_code >> 6) & 0x1f)) .
                          chr(0x80 | ($char_code & 0x3f)) );
                break;
            case !($char_code & 0xffff0000): // 16 bit
                $char = ( chr(0xe0 | (($char_code >> 12) & 0x0f)) .
                          chr(0x80 | (($char_code >> 6) & 0x3f)) .
                          chr(0x80 | ($char_code & 0x3f)) );
                break;
            case !($char_code & 0xffe00000): // 21 bit
                $char = ( chr(0xf0 | (($char_code >> 18) & 0x07)) .
                          chr(0x80 | (($char_code >> 12) & 0x3f)) .
                          chr(0x80 | (($char_code >> 6) & 0x3f)) .
                          chr(0x80 | ($char_code & 0x3f)) );
                break;
            case !($char_code & 0xfc000000): // 26 bit
                $char = ( chr(0xf8 | (($char_code >> 24) & 0x03)) .
                          chr(0x80 | (($char_code >> 18) & 0x3f)) .
                          chr(0x80 | (($char_code >> 12) & 0x3f)) .
                          chr(0x80 | (($char_code >> 6) & 0x3f)) .
                          chr(0x80 | ($char_code & 0x3f)) );
                break;
            default: // 31 bit
                $char = ( chr(0xfc | (($char_code >> 30) & 0x01)) .
                          chr(0x80 | (($char_code >> 24) & 0x3f)) .
                          chr(0x80 | (($char_code >> 18) & 0x3f)) .
                          chr(0x80 | (($char_code >> 12) & 0x3f)) .
                          chr(0x80 | (($char_code >> 6) & 0x3f)) .
                          chr(0x80 | ($char_code & 0x3f)) );
        }
        return $char;
    }


    echo "\n", toUTF8(65), "\n", toUTF8(233), "\n", toUTF8(1252), "\n", toUTF8(20095), "\n";
?>
--EXPECTF--
Warning: The float 4294967168 is not representable as an int, cast occurred in %s on line %d
A

Warning: The float 4294967168 is not representable as an int, cast occurred in %s on line %d

Warning: The float 4294965248 is not representable as an int, cast occurred in %s on line %d
é

Warning: The float 4294967168 is not representable as an int, cast occurred in %s on line %d

Warning: The float 4294965248 is not representable as an int, cast occurred in %s on line %d
Ӥ

Warning: The float 4294967168 is not representable as an int, cast occurred in %s on line %d

Warning: The float 4294965248 is not representable as an int, cast occurred in %s on line %d

Warning: The float 4294901760 is not representable as an int, cast occurred in %s on line %d
乿
