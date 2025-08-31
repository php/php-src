--TEST--
Bug #35176 (include()/require()/*_once() produce wrong error messages about main())
--INI--
html_errors=1
docref_root="/"
error_reporting=4095
--FILE--
<?php
require_once('nonexistent.php');
?>
--EXPECTF--
<br />
<b>Warning</b>:  require_once(nonexistent.php) [<a href='/function.require-once.html'>function.require-once.html</a>]: Failed to open stream: No such file or directory in <b>%sbug35176.php</b> on line <b>2</b><br />
<br />
<b>Fatal error</b>:  Uncaught Error: Failed opening required 'nonexistent.php' (include_path='%s') in %s:%d
Stack trace:
#0 {main}
  thrown in <b>%sbug35176.php</b> on line <b>2</b><br />
