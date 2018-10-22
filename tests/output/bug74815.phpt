--TEST--
Bug #74815 crash with a combination of INI entries at startup
--FILE--
<?php

$php = getenv("TEST_PHP_EXECUTABLE");

echo shell_exec("$php -n -d error_log=".__DIR__."/error_log.tmp -d error_reporting=E_ALL -d log_errors=On -d track_errors=On -v");

?>
==DONE==
--CLEAN--
<?php
unlink(__DIR__.'/error_log.tmp');
?>
--EXPECTF--
Deprecated: Directive 'track_errors' is deprecated in Unknown on line 0
%A
==DONE==
