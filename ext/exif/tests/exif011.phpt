--TEST--
Check for exif_read_data, JPEG with IFD and EXIF data in Motorola byte-order.
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
var_dump(exif_read_data(dirname(__FILE__).'/image011.jpg'));
?>
--EXPECTF--
array(28) {
  [u"FileName"]=>
  unicode(12) "image011.jpg"
  [u"FileDateTime"]=>
  int(%d)
  [u"FileSize"]=>
  int(741)
  [u"FileType"]=>
  int(2)
  [u"MimeType"]=>
  unicode(10) "image/jpeg"
  [u"SectionsFound"]=>
  unicode(19) "ANY_TAG, IFD0, EXIF"
  [u"COMPUTED"]=>
  array(9) {
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
    [u"ApertureFNumber"]=>
    unicode(5) "f/8.0"
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
  unicode(19) "2008:06:19 01:47:53"
  [u"Artist"]=>
  unicode(12) "Eric Stewart"
  [u"Copyright"]=>
  unicode(12) "Eric Stewart"
  [u"Exif_IFD_Pointer"]=>
  int(246)
  [u"ExposureTime"]=>
  unicode(5) "1/125"
  [u"FNumber"]=>
  unicode(3) "8/1"
  [u"ISOSpeedRatings"]=>
  int(80)
  [u"DateTimeOriginal"]=>
  unicode(19) "2008:06:19 01:47:53"
  [u"DateTimeDigitized"]=>
  unicode(19) "2008:06:19 01:47:53"
  [u"MeteringMode"]=>
  int(5)
  [u"LightSource"]=>
  int(4)
  [u"Flash"]=>
  int(7)
  [u"FocalLength"]=>
  unicode(4) "29/5"
  [u"ExifImageWidth"]=>
  int(1)
  [u"ExifImageLength"]=>
  int(1)
}
--CREDIT--
Eric Stewart <ericleestewart@gmail.com>
