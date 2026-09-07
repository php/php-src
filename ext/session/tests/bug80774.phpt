--TEST--
Bug #80774 (session_name() problem with backslash)
--EXTENSIONS--
session
--INI--
session.use_strict_mode=0
session.cookie_samesite=Lax
session.cookie_httponly=1
--FILE--
<?php
session_name("foo\\bar");
session_id('12345');
session_start();
?>
--EXPECTHEADERS--
Set-Cookie: foo\bar=12345; path=/; HttpOnly; SameSite=Lax
--EXPECT--
