--TEST--
FPM: bug75212 - php_value acts like php_admin_value
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
php_admin_value[memory_limit]=32M
php_value[date.timezone]=Europe/London
EOT;

$code = <<<EOT
<?php
echo "Test Start\n";
var_dump(ini_get('memory_limit'), ini_get('date.timezone'));
echo "Test End\n";
EOT;

$ini = <<<EOT
memory_limit=64M
date.timezone=Europe/Paris
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->setUserIni($ini);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectBody([
    'Test Start',
    'string(3) "32M"',
    'string(12) "Europe/Paris"',
    'Test End'
]);
$tester->terminate();
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
