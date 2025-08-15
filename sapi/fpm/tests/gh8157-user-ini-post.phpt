--TEST--
FPM: gh8157 - post related INI settings not applied for .user.ini
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
EOT;

$code = <<<EOT
<?php
var_dump(\$_POST);
EOT;

$ini = <<<EOT
post_max_size=10K
html_errors=off
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->setUserIni($ini);
$tester->start();
$tester->expectLogStartNotices();
$tester
    ->request(
        headers: [ 'CONTENT_TYPE' => 'application/x-www-form-urlencoded'],
        stdin: 'foo=' . str_repeat('a', 20000),
        method: 'POST',
    )
    ->expectBody([
       'Warning: PHP Request Startup: POST Content-Length of 20004 bytes exceeds the limit of 10240 bytes in Unknown on line 0',
        'array(0) {',
        '}',
    ], skipHeadersCheck: true);
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
