--TEST--
Check for exif_thumbnail
--SKIPIF--
<?php if (!extension_loaded("exif")) print "skip";?>
--INI--
magic_quotes_runtime=0
output_handler=
zlib.output_compression=0 
--FILE--
<?php
/*
  test1.jpg is a 1*1 image that does not contain any Exif/Comment information
  test2.jpg is the same image but contains Exif/Comment information and a
            copy of test1.jpg as a thumbnail.
*/
$infile = './ext/exif/tests/test1.jpg';
echo md5_file($infile).'_'.filesize($infile);
$thumb = exif_thumbnail('./ext/exif/tests/test2.jpg');
echo " == ";
echo md5($thumb).'_'.strlen($thumb);
echo "\n";
?>
--EXPECT--
27bbfd9fc10e1e663d749f5225447905_523 == 27bbfd9fc10e1e663d749f5225447905_523
