--TEST--
Test getimagesize() function : basic functionality - load APP info from jpeg.
--SKIPIF--
<?php
	if (!defined("IMAGETYPE_JPEG")) {
		die("skip jpeg format is not available");
	}
?>
--FILE--
<?php
/* Prototype  : proto array getimagesize(string imagefile [, array info])
 * Description: Get the size of an image as 4-element array 
 * Source code: ext/standard/image.c
 * Alias to functions: 
 */

/*
 * Load APP info from jpeg
 */

$arr['this'] = "will";
$arr['all'] = "be destroyed!";
$arr['APP1'] = "and this too";

getimagesize( dirname(__FILE__)."/testAPP.jpg", $arr);

foreach ($arr as $key => $value) {
	echo "$key - length: ". strlen($value) ."; md5: " . md5($value) .  "\n" ;
}

?>
===DONE===
--EXPECTF--
APP1 - length: 1717; md5: 02cbf4ba6640c131422483138c968516
APP2 - length: 7275; md5: f5036ccca2031e8bf932bcbd4aca4355
APP13 - length: 42; md5: 2202998bd05e78bcb419f08c070d6f61
===DONE===
