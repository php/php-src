--TEST--
Bug #30695 (32 bit issues)
--FILE--
<?php
    function toUTF8( $char_code )
    {
        switch ( $char_code )
        {
            case 0:
                $char = chr( 0 );
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
--EXPECT--
A
é
Ӥ
乿
