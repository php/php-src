--TEST--
Check for exif_thumbnail
--SKIPIF--
<?php if (!extension_loaded("exif")) print "skip";?>
--POST--
--GET--
--FILE--
<?php
/*
  test1.jpg is a 1*1 image that does not contain any Exif/Comment information
  test2.jpg is the same image but contains Exif/Comment information and a
            copy of test1.jpg as a thumbnail.
*/
$istat= stat('./ext/exif/tests/test1.jpg');
$fp    = fopen('./ext/exif/tests/test1.jpg','r');
$image = fread($fp,$istat[7]);
echo substr($image,490,5).'_'.$istat[7];
fclose($fp);
$thumb = exif_thumbnail('./ext/exif/tests/test2.jpg');
echo strcmp($image,$thumb) ? 'different' : '_identical_';
echo strlen($thumb);
echo '_'.substr($thumb,490,5);
/* 7GWgw_523_identical_523_7GWgw */
?>
--EXPECT--
7GWgw_523_identical_523_7GWgw
