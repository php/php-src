--TEST--
GH-25051 (TrueType fonts lose their MIME type since the FontKit change)
--EXTENSIONS--
fileinfo
--FILE--
<?php
$finfo = new finfo(FILEINFO_MIME_TYPE);
$description = new finfo();
$filename = __DIR__ . '/gh25051.tmp';

foreach ([
    'TrueType' => ["\x00\x01\x00\x00", 'head'],
    'Apple TrueType' => ['true', 'head'],
    'Type 1 sfnt' => ['typ1', 'head'],
    'FontKit' => ["\x00\x01\x00\x00", 'FK  '],
    'OpenType' => ['OTTO', 'CFF '],
] as $name => [$version, $tag]) {
    /* An sfnt header and a single table directory entry. */
    $font = $version . pack('nnnn', 1, 16, 0, 0)
        . $tag . pack('NNN', 0, 28, 0);
    echo "$name:\n";
    var_dump($finfo->buffer($font));
    file_put_contents($filename, $font);
    var_dump($finfo->file($filename));
    var_dump($finfo->buffer($font, FILEINFO_EXTENSION));
    var_dump($description->buffer($font));
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25051.tmp');
?>
--EXPECT--
TrueType:
string(8) "font/ttf"
string(8) "font/ttf"
string(7) "ttf/tte"
string(40) "TrueType Font data, 1 tables, 1st "head""
Apple TrueType:
string(8) "font/ttf"
string(8) "font/ttf"
string(7) "ttf/tte"
string(40) "TrueType Font data, 1 tables, 1st "head""
Type 1 sfnt:
string(8) "font/ttf"
string(8) "font/ttf"
string(7) "ttf/tte"
string(40) "TrueType Font data, 1 tables, 1st "head""
FontKit:
string(8) "font/ttf"
string(8) "font/ttf"
string(7) "ttf/tte"
string(38) "TrueType (FontKit) Font data, 1 tables"
OpenType:
string(27) "application/vnd.ms-opentype"
string(27) "application/vnd.ms-opentype"
string(3) "???"
string(18) "OpenType font data"
