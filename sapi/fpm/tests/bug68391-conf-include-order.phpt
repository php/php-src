--TEST--
FPM: bug68391 - Configuration inclusion in alphabetical order
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfRoot();
?>
--FILE--
<?php

require_once "tester.inc";

$cfg['main'] = <<<EOT
[global]
error_log = {{FILE:LOG}}
log_level = notice
include = {{INCLUDE:CONF}}
EOT;

$cfg['poolTemplate'] = <<<EOT
[%name%]
listen = {{ADDR[%name%]}}
user = foo
pm = ondemand
pm.max_children = 5
EOT;

$cfg['names'] = ['cccc', 'aaaa', 'eeee', 'dddd', 'bbbb'];

$tester = new FPM\Tester($cfg);
$tester->start();
$userMessage = "'user' directive is ignored when FPM is not running as root";
$tester->expectLogNotice($userMessage, 'aaaa');
$tester->expectLogNotice($userMessage, 'bbbb');
$tester->expectLogNotice($userMessage, 'cccc');
$tester->expectLogNotice($userMessage, 'dddd');
$tester->expectLogNotice($userMessage, 'eeee');
$tester->expectLogStartNotices();
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
