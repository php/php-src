--TEST--
FPM: bug72185 - FastCGI empty frame incorrectly created
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
pm.max_children = 2
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 2
catch_workers_output = yes
decorate_workers_output = no
php_value[html_errors] = false
EOT;

$code = <<<'EOT'
<?php
for ($i=0; $i < 167; $i++) {
    error_log('PHP is the best programming language');
}
echo 'end';
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectBody('end');
for ($i=0; $i < 167; $i++) {
    $tester->expectLogNotice("PHP message: PHP is the best programming language");
}
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
