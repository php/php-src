--TEST--
Check for exif_read_data
--SKIPIF--
<?php if (!extension_loaded("exif")) print "skip";?>
--FILE--
<?php
/*
  test1.jpg is a 1*1 image that does not contain any Exif/Comment information
  test2.jpg is the same image but contains Exif/Comment information and a
            copy of test1.jpg as a thumbnail.
*/
var_dump(exif_read_data('./ext/exif/tests/test2.jpg','',true,false));
?>
--EXPECTF--
array(5) {
  ["FILE"]=>
  array(6) {
    ["FileName"]=>
    string(9) "test2.jpg"
    ["FileDateTime"]=>
    int(%d)
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
    string(16) "Exif test image."
    ["UserCommentEncoding"]=>
    string(5) "ASCII"
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
    string(5) "ASCII"
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