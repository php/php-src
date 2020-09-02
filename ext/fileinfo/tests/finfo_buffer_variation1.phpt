--TEST--
Test finfo_buffer() function : basic functionality
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php
$magicFile = __DIR__ . DIRECTORY_SEPARATOR . 'magic';

$options = array(
    FILEINFO_NONE,
    FILEINFO_MIME,
);

$buffers = array(
    "Regular string here",
    "\177ELF",
    "\000\000\0001\000\000\0000\000\000\0000\000\000\0002\000\000\0000\000\000\0000\000\000\0003",
    "\x55\x7A\x6E\x61",
    "id=ImageMagick\x0a\x0c\x0a:\x1a",
    "RIFFüîò^BAVI LISTv",
);

echo "*** Testing finfo_buffer() : variation functionality with oo interface ***\n";

foreach( $options as $option ) {
    $finfo = new finfo( $option, $magicFile );
    foreach( $buffers as $string ) {
        var_dump( $finfo->buffer( $string, $option ) );
    }
}

?>
--EXPECT--
*** Testing finfo_buffer() : variation functionality with oo interface ***
string(36) "ASCII text, with no line terminators"
string(3) "ELF"
string(22) "old ACE/gr binary file"
string(12) "xo65 object,"
string(15) "MIFF image data"
string(25) "RIFF (little-endian) data"
string(28) "text/plain; charset=us-ascii"
string(26) "text/plain; charset=ebcdic"
string(40) "application/octet-stream; charset=binary"
string(28) "text/plain; charset=us-ascii"
string(28) "image/x-miff; charset=binary"
string(25) "text/plain; charset=utf-8"
