--TEST--
Check for exif_read_data, magic_quotes_runtime
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
magic_quotes_runtime=1
--FILE--
<?php
/*
  test1.jpg is a 1*1 image that does not contain any Exif/Comment information
  test2.jpg is the same image but contains Exif/Comment information and a
            copy of test1.jpg as a thumbnail.
  test6.jpg is the same as test2.jpg but with a UNICODE UserComment: &Auml;&Ouml;&&Uuml;&szlig;&auml;&ouml;&uuml;
*/
var_dump(exif_read_data('./ext/exif/tests/test6.jpg','',true,false));
?>
--EXPECTF--
array(5) {
  ["FILE"]=>
  array(6) {
    ["FileName"]=>
    string(9) "test6.jpg"
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
    string(24) "width=\"1\" height=\"1\""
    ["Height"]=>
    int(1)
    ["Width"]=>
    int(1)
    ["IsColor"]=>
    int(1)
    ["ByteOrderMotorola"]=>
    int(1)
    ["UserComment"]=>
    string(16) "Hallo \'Du\'+da!"
    ["UserCommentEncoding"]=>
    string(5) "ASCII"
    ["Copyright"]=>
    string(45) "Photo \"M. Boerger\"., Edited \'M. Boerger\'."
    ["Copyright.Photographer"]=>
    string(21) "Photo \"M. Boerger\"."
    ["Copyright.Editor"]=>
    string(22) "Edited \'M. Boerger\'."
    ["Thumbnail.FileType"]=>
    int(2)
    ["Thumbnail.MimeType"]=>
    string(10) "image/jpeg"
  }
  ["IFD0"]=>
  array(2) {
    ["Copyright"]=>
    string(21) "Photo \"M. Boerger\"."
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
    string(13) "Comment \"1\""
    [1]=>
    string(13) "Comment \'2\'"
    [2]=>
    string(13) "Comment #3end"
  }
}
