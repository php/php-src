--TEST--
Bug #44098 (imap_utf8() returns only capital letters)
--EXTENSIONS--
imap
--FILE--
<?php
$exp = 'Luzon®14 dot CoM';
$res = imap_utf8('=?iso-8859-1?b?THV6b26uMTQ=?= dot CoM');
var_dump($res);

?>
--EXPECT--
string(17) "Luzon®14 dot CoM"
