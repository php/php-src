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
var_dump(exif_read_data(dirname(__FILE__).'/test3.jpg','',true,false));
?>
--EXPECTF--
array(5) {
  [u"FILE"]=>
  array(6) {
    [u"FileName"]=>
    unicode(9) "test3.jpg"
    [u"FileDateTime"]=>
    int(%s)
    [u"FileSize"]=>
    int(1240)
    [u"FileType"]=>
    int(2)
    [u"MimeType"]=>
    unicode(10) "image/jpeg"
    [u"SectionsFound"]=>
    unicode(33) "ANY_TAG, IFD0, THUMBNAIL, COMMENT"
  }
  [u"COMPUTED"]=>
  array(12) {
    [u"html"]=>
    unicode(20) "width="1" height="1""
    [u"Height"]=>
    int(1)
    [u"Width"]=>
    int(1)
    [u"IsColor"]=>
    int(1)
    [u"ByteOrderMotorola"]=>
    int(1)
    [u"UserComment"]=>
    unicode(7) "ÄÖÜßäöü"
    [u"UserCommentEncoding"]=>
    unicode(7) "UNICODE"
    [u"Copyright"]=>
    unicode(41) "Photo (c) M.Boerger, Edited by M.Boerger."
    [u"Copyright.Photographer"]=>
    unicode(19) "Photo (c) M.Boerger"
    [u"Copyright.Editor"]=>
    unicode(20) "Edited by M.Boerger."
    [u"Thumbnail.FileType"]=>
    int(2)
    [u"Thumbnail.MimeType"]=>
    unicode(10) "image/jpeg"
  }
  [u"IFD0"]=>
  array(2) {
    [u"Copyright"]=>
    unicode(19) "Photo (c) M.Boerger"
    [u"UserComment"]=>
    unicode(7) "UNICODE"
  }
  [u"THUMBNAIL"]=>
  array(2) {
    [u"JPEGInterchangeFormat"]=>
    int(134)
    [u"JPEGInterchangeFormatLength"]=>
    int(523)
  }
  [u"COMMENT"]=>
  array(3) {
    [0]=>
    unicode(11) "Comment #1."
    [1]=>
    unicode(11) "Comment #2."
    [2]=>
    unicode(13) "Comment #3end"
  }
}
