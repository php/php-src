--TEST--
Check for exif_read_data, Unicode WinXP tags	
--SKIPIF--
<?php if (!extension_loaded("exif")) print "skip";?>
--POST--
--GET--
--FILE--
<?php
/*
  test4.jpg is a 1*1 image that contains Exif tags written by WindowsXP
*/
chdir($_ENV['PHP_DIR']);
ini_alter('exif.decode_unicode_motorola', 'UCS-2BE');
ini_alter('exif.encode_unicode', 'ISO-8859-15');
$image  = exif_read_data('./ext/exif/tests/test4.jpg','',true,false);
$accept = '';
foreach($image as $idx=>$section) {
	$accept .= $section;
	foreach($section as $name=>$value) {
		if  ( $idx!='FILE' || $name!='FileDateTime') {
			$accept .= substr($name,0,2);
			$accept .= $value;
		}
	}
}
echo $accept;
?>
--EXPECT--
ArrayFitest4.jpgFi713Fi2Miimage/jpegSeANY_TAG, IFD0, WINXPArrayhtwidth="1" height="1"He1Wi1Is1By0ArraySuArrayKeArrayAuArrayCoArrayTiArrayArraySuSubject...KeKeywords...AuRui CarmoCoComments
Line2
Line3
Line4TiTitle...