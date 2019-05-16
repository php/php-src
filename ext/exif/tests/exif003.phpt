--TEST--
Check for exif_read_data, Unicode user comment
--SKIPIF--
<?php
	if (!extension_loaded('exif')) die('skip exif extension not available');
	if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
	if (!defined("EXIF_USE_MBSTRING") || !EXIF_USE_MBSTRING) die ('skip mbstring loaded by dl');
?>
--INI--
output_handler=
zlib.output_compression=0
exif.decode_unicode_motorola=UCS-2BE
exif.encode_unicode=ISO-8859-15
--FILE--
<?php
/*
  test1.jpg is a 1*1 image that does not contain any Exif/Comment information
  test2.jpg is the same image but contains Exif/Comment information and a
            copy of test1.jpg as a thumbnail.
  test3.jpg is the same as test2.jpg but with a UNICODE UserComment: &Auml;&Ouml;&&Uuml;&szlig;&auml;&ouml;&uuml;
*/
var_dump(exif_read_data(__DIR__.'/test3.jpg','',true,false));
?>
--EXPECTF--
array(5) {
  ["FILE"]=>
  array(6) {
    ["FileName"]=>
    string(9) "test3.jpg"
    ["FileDateTime"]=>
    int(%s)
    ["FileSize"]=>
    int(1240)
    ["FileType"]=>
    int(2)
    ["MimeType"]=>
    string(10) "image/jpeg"
    ["SectionsFound"]=>
    string(33) "ANY_TAG, IFD0, THUMBNAIL, COMMENT"
  }
  ["COMPUTED"]=>
  array(12) {
    ["html"]=>
    string(20) "width="1" height="1""
    ["Height"]=>
    int(1)
    ["Width"]=>
    int(1)
    ["IsColor"]=>
    int(1)
    ["ByteOrderMotorola"]=>
    int(1)
    ["UserComment"]=>
    string(7) "ÄÖÜßäöü"
    ["UserCommentEncoding"]=>
    string(7) "UNICODE"
    ["Copyright"]=>
    string(41) "Photo (c) M.Boerger, Edited by M.Boerger."
    ["Copyright.Photographer"]=>
    string(19) "Photo (c) M.Boerger"
    ["Copyright.Editor"]=>
    string(20) "Edited by M.Boerger."
    ["Thumbnail.FileType"]=>
    int(2)
    ["Thumbnail.MimeType"]=>
    string(10) "image/jpeg"
  }
  ["IFD0"]=>
  array(2) {
    ["Copyright"]=>
    string(19) "Photo (c) M.Boerger"
    ["UserComment"]=>
    string(7) "UNICODE"
  }
  ["THUMBNAIL"]=>
  array(2) {
    ["JPEGInterchangeFormat"]=>
    int(134)
    ["JPEGInterchangeFormatLength"]=>
    int(523)
  }
  ["COMMENT"]=>
  array(3) {
    [0]=>
    string(11) "Comment #1."
    [1]=>
    string(11) "Comment #2."
    [2]=>
    string(13) "Comment #3end"
  }
}
