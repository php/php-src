--TEST--
Check for exif_thumbnail
--SKIPIF--
<?php if (!extension_loaded("exif")) print "skip";?>
--INI--
magic_quotes_runtime=0
--FILE--
<?php
/*
  test1.jpg is a 1*1 image that does not contain any Exif/Comment information
  test2.jpg is the same image but contains Exif/Comment information and a
            copy of test1.jpg as a thumbnail.
*/
if (function_exists("ob_end_clean")) ob_end_clean();
$infile= './ext/exif/tests/test1.jpg';
$fp    = fopen($infile,'rb');
$image = fread($fp,filesize($infile));
//$image = stripslashes($image);
echo md5($image).'_'.filesize($infile);
fclose($fp);
$thumb = exif_thumbnail('./ext/exif/tests/test2.jpg');
echo strcmp($image,$thumb) ? '_different_' : '_identical_';
echo strlen($thumb).'_'.md5($thumb);
echo "\n";
?>
--EXPECT--
27bbfd9fc10e1e663d749f5225447905_523_identical_523_27bbfd9fc10e1e663d749f5225447905
