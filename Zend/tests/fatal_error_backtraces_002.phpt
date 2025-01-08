--TEST--
Fatal error backtrace w/ sensitive parameters
--INI--
fatal_error_backtraces=On
--FILE--
<?php

function trigger_fatal(#[\SensitiveParameter] $unused) {
    eval("class Foo {}; class Foo {}");
}

trigger_fatal("bar");
?>
--EXPECTF--
Fatal error: Cannot redeclare class Foo (%s) in %s : eval()'d code on line %d
Stack trace:
#0 %sfatal_error_backtraces_002.php(%d): eval()
#1 %sfatal_error_backtraces_002.php(%d): trigger_fatal(Object(SensitiveParameterValue))
#2 {main}
