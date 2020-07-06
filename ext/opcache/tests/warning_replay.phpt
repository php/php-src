--TEST--
Opcache should replay compilation warnings if opcache.record_warnings=1
--INI--
opcache.record_warnings=1
--FILE--
<?php

require __DIR__ . '/warning_replay.inc';
require __DIR__ . '/warning_replay.inc';

?>
--EXPECTF--
Warning: "unknown" declaration is unsupported in %swarning_replay.inc on line 3

Warning: "unknown" declaration is unsupported in %swarning_replay.inc on line 3
