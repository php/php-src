--TEST--
FriBidi charsets
--SKIPIF--
<?php if (!extension_loaded("fribidi")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
	error_reporting(E_ALL ^ E_NOTICE);

	echo FRIBIDI_CHARSET_8859_8."\n";
	echo FRIBIDI_CHARSET_8859_6."\n";
	echo FRIBIDI_CHARSET_UTF8."\n";
	echo FRIBIDI_CHARSET_CP1255."\n";
	echo FRIBIDI_CHARSET_CP1256."\n";
	echo FRIBIDI_CHARSET_ISIRI_3342."\n";
	echo FRIBIDI_CHARSET_CAP_RTL."\n";
?>
--EXPECT--
4
3
1
5
6
7
2
