--TEST--
iconv_mime_decode() (quoted-printable)
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--INI--
iconv.internal_charset=iso-8859-1
--FILE--
<?php
$header = <<< HERE
Subject: =?ISO-8859-1?Q?Pr=FCfung?=
	=?ISO-8859-1*de_DE?Q?=20Pr=FCfung?= 
 	 =?ISO-8859-2?Q?k=F9=D4=F1=D3let?=
HERE;
$result = iconv_mime_decode($header, "UTF-8");
printf("(%d) \"%s\"\n", iconv_strlen($result, "UTF-8"), $result);
--EXPECT--
(33) "Subject: Prüfung Prüfung kůÔńÓlet"
