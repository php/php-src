--TEST--
FPM: bug72573 - HTTP_PROXY - CVE-2016-5385
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
echo "Test Start\n";
var_dump(
    @\$_SERVER["HTTP_PROXY"],
    \$_SERVER["HTTP_FOO"],
    getenv("HTTP_PROXY"),
    getenv("HTTP_FOO")
);
echo "Test End\n";
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester
    ->request(
        '',
        [
            'HTTP_FOO' => 'BAR',
            'HTTP_PROXY' => 'BADPROXY',
        ]
    )
    ->expectBody(
        [
            'Test Start',
            'NULL',
            'string(3) "BAR"',
            'bool(false)',
            'string(3) "BAR"',
            'Test End'
        ]
    );
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
