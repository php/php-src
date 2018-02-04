--TEST--
Check for exif_read_data, JPEG with IFD data in Intel byte-order.
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
var_dump(exif_read_data(dirname(__FILE__).'/image008.jpg'));
?>
--EXPECTF--
array(16) {
  ["FileName"]=>
  string(12) "image008.jpg"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(%d)
  ["FileType"]=>
  int(2)
  ["MimeType"]=>
  string(10) "image/jpeg"
  ["SectionsFound"]=>
  string(13) "ANY_TAG, IFD0"
  ["COMPUTED"]=>
  array(8) {
    ["html"]=>
    string(20) "width="1" height="1""
    ["Height"]=>
    int(1)
    ["Width"]=>
    int(1)
    ["IsColor"]=>
    int(1)
    ["ByteOrderMotorola"]=>
    int(0)
    ["Copyright"]=>
    string(24) "Eric Stewart, Hex Editor"
    ["Copyright.Photographer"]=>
    string(12) "Eric Stewart"
    ["Copyright.Editor"]=>
    string(10) "Hex Editor"
  }
  ["ImageDescription"]=>
  string(15) "My description."
  ["Make"]=>
  string(11) "OpenShutter"
  ["Model"]=>
  string(8) "OS 1.0.0"
  ["XResolution"]=>
  string(4) "72/1"
  ["YResolution"]=>
  string(4) "72/1"
  ["ResolutionUnit"]=>
  int(2)
  ["DateTime"]=>
  string(19) "2008:06:19 01:47:53"
  ["Artist"]=>
  string(12) "Eric Stewart"
  ["Copyright"]=>
  string(12) "Eric Stewart"
}
--CREDITS--
Eric Stewart <ericleestewart@gmail.com>
