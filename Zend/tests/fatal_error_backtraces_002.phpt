--TEST--
Fatal error backtrace w/ sensitive parameters
--FILE--
<?php

ini_set('fatal_error_backtraces', true);

function oom(#[\SensitiveParameter] $unused) {
    $argv[1] = "stdClass";

    include __DIR__ . '/new_oom.inc';
}

oom("foo");

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted at %s:%d (tried to allocate %d bytes) in %snew_oom.inc on line %d
Stack trace:
#0 %snew_oom.inc(%d): ReflectionClass->newInstanceWithoutConstructor()
#1 %sfatal_error_backtraces_002.php(%d): include(%s)
#2 %sfatal_error_backtraces_002.php(%d): oom(Object(SensitiveParameterValue))
#3 {main}
