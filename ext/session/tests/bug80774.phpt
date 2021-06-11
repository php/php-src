--TEST--
Bug #80774 (session_name() problem with backslash)
--EXTENSIONS--
session
--FILE--
<?php
session_name("foo\\bar");
session_id('12345');
session_start();
?>
--EXPECTHEADERS--
Set-Cookie: foo\bar=12345; path=/
--EXPECT--
