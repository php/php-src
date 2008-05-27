--TEST--
Check for exif_read_data
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
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
var_dump(exif_read_data(dirname(__FILE__).'/test2.jpg','',true,false));
?>
--EXPECTF--
array(5) {
  [u"FILE"]=>
  array(6) {
    [u"FileName"]=>
    string(9) "test2.jpg"
    [u"FileDateTime"]=>
    int(%d)
    [u"FileSize"]=>
    int(1240)
    [u"FileType"]=>
    int(2)
    [u"MimeType"]=>
    string(10) "image/jpeg"
    [u"SectionsFound"]=>
    string(33) "ANY_TAG, IFD0, THUMBNAIL, COMMENT"
  }
  [u"COMPUTED"]=>
  array(12) {
    [u"html"]=>
    string(20) "width="1" height="1""
    [u"Height"]=>
    int(1)
    [u"Width"]=>
    int(1)
    [u"IsColor"]=>
    int(1)
    [u"ByteOrderMotorola"]=>
    int(1)
    [u"UserComment"]=>
    string(16) "Exif test image."
    [u"UserCommentEncoding"]=>
    string(5) "ASCII"
    [u"Copyright"]=>
    string(41) "Photo (c) M.Boerger, Edited by M.Boerger."
    [u"Copyright.Photographer"]=>
    string(19) "Photo (c) M.Boerger"
    [u"Copyright.Editor"]=>
    string(20) "Edited by M.Boerger."
    [u"Thumbnail.FileType"]=>
    int(2)
    [u"Thumbnail.MimeType"]=>
    string(10) "image/jpeg"
  }
  [u"IFD0"]=>
  array(2) {
    [u"Copyright"]=>
    string(19) "Photo (c) M.Boerger"
    [u"UserComment"]=>
    string(5) "ASCII"
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
    string(11) "Comment #1."
    [1]=>
    string(11) "Comment #2."
    [2]=>
    string(13) "Comment #3end"
  }
}
