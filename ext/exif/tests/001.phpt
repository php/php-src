--TEST--
Check for exif_read_data
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
$image  = exif_read_data('./ext/exif/tests/test2.jpg','',true,false);
$accept = '';
foreach($image as $idx=>$section) {
	$accept .= $section;
	foreach($section as $name=>$value) {
		$accept .= substr($name,0,2);
		$accept .= $value;
	}
}
echo $accept;
?>
--EXPECT--
ArrayFitest2.jpgFi1015448798Fi1240Fi2SeANY_TAG, IFD0, THUMBNAIL, COMMENTArrayhtwidth="1" height="1"He1Wi1Is1UsExif test image.UsASCIICoPhoto (c) M.Boerger, Edited by M.Boerger.CoPhoto (c) M.BoergerCoPhoto (c) M.BoergerArrayCoPhoto (c) M.BoergerUsASCIIArrayJP134JP523Array0Comment #1.1Comment #2.2Comment #3end