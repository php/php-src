--TEST--
Check for read_exif_data default behaviour
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php

/* Prototype  : array read_exif_data  ( string $filename  [, string $sections  [, bool $arrays  [, bool $thumbnail  ]]] )
 * Description: Alias of exif_read_data()
 * Source code: ext/exif/exif.c
*/
echo "*** Testing read_exif_data() : basic functionality ***\n";

print_r(read_exif_data(__DIR__.'/test2私はガラスを食べられます.jpg'));
?>
===Done===
--EXPECTF--
*** Testing read_exif_data() : basic functionality ***

Deprecated: Function read_exif_data() is deprecated in %sexif_read_exif_data_basic-mb.php on line 9
Array
(
    [FileName] => test2私はガラスを食べられます.jpg
    [FileDateTime] => %d
    [FileSize] => 1240
    [FileType] => 2
    [MimeType] => image/jpeg
    [SectionsFound] => ANY_TAG, IFD0, THUMBNAIL, COMMENT
    [COMPUTED] => Array
        (
            [html] => width="1" height="1"
            [Height] => 1
            [Width] => 1
            [IsColor] => 1
            [ByteOrderMotorola] => 1
            [UserComment] => Exif test image.
            [UserCommentEncoding] => ASCII
            [Copyright] => Photo (c) M.Boerger, Edited by M.Boerger.
            [Copyright.Photographer] => Photo (c) M.Boerger
            [Copyright.Editor] => Edited by M.Boerger.
            [Thumbnail.FileType] => 2
            [Thumbnail.MimeType] => image/jpeg
        )

    [Copyright] => Photo (c) M.Boerger
    [UserComment] => ASCII
    [THUMBNAIL] => Array
        (
            [JPEGInterchangeFormat] => 134
            [JPEGInterchangeFormatLength] => 523
        )

    [COMMENT] => Array
        (
            [0] => Comment #1.
            [1] => Comment #2.
            [2] => Comment #3end
        )

)
===Done===
