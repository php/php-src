--TEST--
Check for exif_read_data
--SKIPIF--
<?php if (!extension_loaded("exif") || !function_exists('mb_convert_encoding')) print "skip";?>
--POST--
--GET--
--FILE--
<?php
/*
  test1.jpg is a 1*1 image that does not contain any Exif/Comment information
  test2.jpg is the same image but contains Exif/Comment information and a
            copy of test1.jpg as a thumbnail.
  test3.jpg is the same as test2.jpg but with a UNICODE USerComment: &Auml;&Ouml;&&Uuml;&szlig;&auml;&ouml;&uuml;
*/
$image  = exif_read_data('./ext/exif/tests/test3.jpg','',true,false);
$accept = '';
foreach($image as $idx=>$section) {
	$accept .= $section;
	if ($idx=="COMPUTED" && array_key_exists("UserCommentEncoding",$section) && $section["UserCommentEncoding"]=="UNICODE") {
		$section["UserComment"] = mb_convert_encoding($section["UserComment"],"ISO-8859-1","UCS-2BE");
	}
	foreach($section as $name=>$value) {
		if  ($idx!='FILE' || $name!='FileDateTime') {
			$accept .= substr($name,0,2);
			$accept .= $value;
		}
	}
}
echo $accept;
?>
--EXPECT--
ArrayFitest3.jpgFi1237SeCOMPUTED, ANY_TAG, IFD0, THUMBNAIL, COMMENT, EXIFArrayCoPhoto (c) M.BoergerCoEdited by M.Boerger.htwidth="1" height="1"He1Wi1Is1Usƒ÷‹ﬂ‰ˆ¸UsUNICODEArrayCoPhoto (c) M.BoergerUsUNICODEArrayJP134JP523Array0Comment #1.1Comment #2.2Comment #3end