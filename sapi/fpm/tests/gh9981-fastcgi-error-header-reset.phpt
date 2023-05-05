--TEST--
FPM: gh9981 - fastcgi.error_header is not reset
--SKIPIF--
<?php
include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 1
catch_workers_output = yes
EOT;

$code = <<<EOT
<?php
if (isset(\$_GET['q'])) {
    echo 'ok';
} else {
    d();
}
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'fastcgi.error_header' => '"HTTP/1.1 500 PHP Error"',
    'output_buffering'     => 4096,
]);
$tester->expectLogStartNotices();
$tester->request()->expectStatus('500 PHP Error');
$tester->request('q=1')->expectNoStatus();
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->expectNoLogPattern('/Cannot modify header information/');
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
