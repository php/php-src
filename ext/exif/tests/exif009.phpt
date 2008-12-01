--TEST--
Check for exif_read_data, JPEG with IFD data in Motorola byte-order.
--CREDIT--
Eric Stewart <ericleestewart@gmail.com>
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
var_dump(exif_read_data(dirname(__FILE__).'/image009.jpg'));
?>
--EXPECTF--
array(16) {
  [u"FileName"]=>
  unicode(12) "image009.jpg"
  [u"FileDateTime"]=>
  int(%d)
  [u"FileSize"]=>
  int(%d)
  [u"FileType"]=>
  int(2)
  [u"MimeType"]=>
  unicode(10) "image/jpeg"
  [u"SectionsFound"]=>
  unicode(13) "ANY_TAG, IFD0"
  [u"COMPUTED"]=>
  array(8) {
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
    [u"Copyright"]=>
    unicode(24) "Eric Stewart, Hex Editor"
    [u"Copyright.Photographer"]=>
    unicode(12) "Eric Stewart"
    [u"Copyright.Editor"]=>
    unicode(10) "Hex Editor"
  }
  [u"ImageDescription"]=>
  unicode(15) "My description."
  [u"Make"]=>
  unicode(11) "OpenShutter"
  [u"Model"]=>
  unicode(8) "OS 1.0.0"
  [u"XResolution"]=>
  unicode(4) "72/1"
  [u"YResolution"]=>
  unicode(4) "72/1"
  [u"ResolutionUnit"]=>
  int(2)
  [u"DateTime"]=>
  unicode(%d) "%s"
  [u"Artist"]=>
  unicode(12) "Eric Stewart"
  [u"Copyright"]=>
  unicode(12) "Eric Stewart"
}

