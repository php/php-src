--TEST--
Bug #74725: html_errors=1 breaks unhandled exceptions
--FILE--
<?php
ini_set('display_errors', 1);
ini_set('html_errors', 1);
ini_set('default_charset', "Windows-1251");
throw new Exception("\xF2\xE5\xF1\xF2");
// Note to test reader: this file is in Windows-1251 (vim: `:e ++enc=cp1251`)
?>
--EXPECTF--
<br />
<b>Fatal error</b>:  Uncaught Exception: тест in %s:5
Stack trace:
#0 {main}
  thrown in <b>%s</b> on line <b>5</b><br />
