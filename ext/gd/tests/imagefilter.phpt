--TEST--
imagefilter() function test
--SKIPIF--
<?php 
	if (!extension_loaded('gd')) {
		die("skip gd extension not avaliable.");
	}
	if (!GD_BUNDLED) {
		die("skip this test requires bundled gd library.");
	}
?>
--FILE--
<?php
$no_arg_filters = array(
	"IMG_FILTER_NEGATE",
	"IMG_FILTER_GRAYSCALE",
	"IMG_FILTER_EDGEDETECT",
	"IMG_FILTER_GAUSSIAN_BLUR", 
	"IMG_FILTER_SELECTIVE_BLUR",
	"IMG_FILTER_EMBOSS",
	"IMG_FILTER_MEAN_REMOVAL"
);

$SAVE_DIR = dirname(__FILE__);
$SOURCE_IMG = $SAVE_DIR . "/test.png";
	
	foreach ($no_arg_filters as $filt) {
		$im = imagecreatefrompng($SOURCE_IMG);
		if (imagefilter($im, constant($filt))) {
			imagepng($im, $SAVE_DIR. $filt. ".png");
			echo "$filt success\n";
			echo md5_file($SAVE_DIR. $filt. ".png") . "\n";
			@unlink($SAVE_DIR. $filt. ".png");
		} else {
			echo "$filt failed\n";
		}
	} 

	$im = imagecreatefrompng($SOURCE_IMG);
	
	if (imagefilter($im, IMG_FILTER_SMOOTH, -1924.124)) {
		imagepng($im, $SAVE_DIR . "IMG_FILTER_SMOOTH.png");
		echo "IMG_FILTER_SMOOTH success\n";
		echo md5_file($SAVE_DIR . "IMG_FILTER_SMOOTH.png") . "\n";
		@unlink($SAVE_DIR . "IMG_FILTER_SMOOTH.png");
	} else {
		echo "IMG_FILTER_SMOOTH failed\n";
	}
	
	$im = imagecreatefrompng($SOURCE_IMG);
	
	if (imagefilter($im, IMG_FILTER_COLORIZE, -127.12, -127.98, 127)) {
		imagepng($im, $SAVE_DIR . "IMG_FILTER_COLORIZE.png");
		echo "IMG_FILTER_COLORIZE success\n";
		echo md5_file($SAVE_DIR . "IMG_FILTER_COLORIZE.png") . "\n";
		unlink($SAVE_DIR . "IMG_FILTER_COLORIZE.png");
	} else {
		echo "IMG_FILTER_COLORIZE failed\n";
	}
	
	$im = imagecreatefrompng($SOURCE_IMG);
	
	if (imagefilter($im, IMG_FILTER_CONTRAST, -90)) {
		imagepng($im, $SAVE_DIR . "IMG_FILTER_CONTRAST.png");
		echo "IMG_FILTER_CONTRAST success\n";
		echo md5_file($SAVE_DIR . "IMG_FILTER_CONTRAST.png") . "\n";
		unlink($SAVE_DIR . "IMG_FILTER_CONTRAST.png");
	} else {
		echo "IMG_FILTER_CONTRAST failed\n";
	}

	$im = imagecreatefrompng($SOURCE_IMG);
	
	if (imagefilter($im, IMG_FILTER_BRIGHTNESS, 98)) {
		imagepng($im, $SAVE_DIR . "IMG_FILTER_BRIGHTNESS.png");
		echo "IMG_FILTER_BRIGHTNESS success\n";
		echo md5_file($SAVE_DIR . "IMG_FILTER_BRIGHTNESS.png") . "\n";
		unlink($SAVE_DIR . "IMG_FILTER_BRIGHTNESS.png");
	} else {
		echo "IMG_FILTER_BRIGHTNESS failed\n";
	}
?>
--EXPECT--
IMG_FILTER_NEGATE success
791b480b940ca777c541a6541a9a21ee
IMG_FILTER_GRAYSCALE success
b6c77efe4f07916b889671a6d58a7646
IMG_FILTER_EDGEDETECT success
4a93e345325edb6e168fc3b603461a42
IMG_FILTER_GAUSSIAN_BLUR success
731a19e0f796ce853879761228f25a06
IMG_FILTER_SELECTIVE_BLUR success
740e8618e131d1e9e61fa4f891e2e4bd
IMG_FILTER_EMBOSS success
8e43cfced8d795bbce048a0402091b24
IMG_FILTER_MEAN_REMOVAL success
e8ae73918566861d1c707df35c171317
IMG_FILTER_SMOOTH success
4bda59ccda7b531f19be13fdda7570c3
IMG_FILTER_COLORIZE success
b7d1b39c3e7d98759ccccda4437ee125
IMG_FILTER_CONTRAST success
837db2a6295b64b0ce3b27621e256b26
IMG_FILTER_BRIGHTNESS success
15d795db86637150f6cfc5f98d074dea
