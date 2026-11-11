--TEST--
NUL bytes in fatal error messages are not truncated with html_errors
--INI--
html_errors=1
--FILE--
<?php
new class { abstract public $p { get; } };
?>
--EXPECTF--
<br />
<b>Fatal error</b>:  Class class@anonymous%0%s must implement 1 abstract method (class@anonymous%0%s::$p::get) in <b>%s</b> on line <b>%d</b><br />
