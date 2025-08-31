--TEST--
Check for exif_read_data default behaviour
--EXTENSIONS--
exif
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
/*
  test1.jpg is a 1*1 image that does not contain any Exif/Comment information
  test2.jpg is the same image but contains Exif/Comment information and a
            copy of test1.jpg as a thumbnail.
*/
print_r(exif_read_data(__DIR__.'/test2.jpg'));
?>
--EXPECTF--
Array
(
    [FileName] => test2.jpg
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
