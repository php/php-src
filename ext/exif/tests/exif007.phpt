--TEST--
Check for exif_read_data, baseline JPEG with no IFD, EXIF, GPS or Interoperability data in Intel byte-order.
--CREDIT--
Eric Stewart <ericleestewart@gmail.com>
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
var_dump(exif_read_data(dirname(__FILE__).'/image007.jpg'));
?>
--EXPECTF--
array(7) {
  [u"FileName"]=>
  unicode(12) "image007.jpg"
  [u"FileDateTime"]=>
  int(%d)
  [u"FileSize"]=>
  int(%d)
  [u"FileType"]=>
  int(2)
  [u"MimeType"]=>
  unicode(10) "image/jpeg"
  [u"SectionsFound"]=>
  unicode(0) ""
  [u"COMPUTED"]=>
  array(4) {
    [u"html"]=>
    unicode(20) "width="1" height="1""
    [u"Height"]=>
    int(1)
    [u"Width"]=>
    int(1)
    [u"IsColor"]=>
    int(1)
  }
}

