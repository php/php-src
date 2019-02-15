--TEST--
phpt EXTENSIONS directive with nonexistent shared module
--INI--
error_log=
display_startup_errors=1
display_errors=1
--EXTENSIONS--
nonexistentsharedmodule
--FILE--
<?php
--EXPECTF--
PHP Warning:  PHP Startup: Unable to load dynamic library '%snonexistentsharedmodule.%s' %A
