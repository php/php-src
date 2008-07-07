--TEST--
Check for exif_read_data, JPEG with IFD0, EXIF, INTEROP data in Intel byte-order.
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
var_dump(exif_read_data(dirname(__FILE__).'/image024.jpg'));
?>
--EXPECTF--
array(14) {
  [u"FileName"]=>
  unicode(12) "image024.jpg"
  [u"FileDateTime"]=>
  int(1215471672)
  [u"FileSize"]=>
  int(417)
  [u"FileType"]=>
  int(2)
  [u"MimeType"]=>
  unicode(10) "image/jpeg"
  [u"SectionsFound"]=>
  unicode(28) "ANY_TAG, IFD0, EXIF, INTEROP"
  [u"COMPUTED"]=>
  array(5) {
    [u"html"]=>
    unicode(20) "width="1" height="1""
    [u"Height"]=>
    int(1)
    [u"Width"]=>
    int(1)
    [u"IsColor"]=>
    int(1)
    [u"ByteOrderMotorola"]=>
    int(0)
  }
  [u"Exif_IFD_Pointer"]=>
  int(26)
  [u"InteroperabilityOffset"]=>
  int(44)
  [u"InterOperabilityIndex"]=>
  unicode(3) "R98"
  [u"InterOperabilityVersion"]=>
  unicode(4) "0100"
  [u"RelatedFileFormat"]=>
  unicode(12) "image024.jpg"
  [u"RelatedImageWidth"]=>
  int(1)
  [u"RelatedImageHeight"]=>
  int(1)
}
--CREDIT--
Eric Stewart <ericleestewart@gmail.com>
