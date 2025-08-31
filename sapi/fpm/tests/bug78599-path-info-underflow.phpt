--TEST--
FPM: bug78599 - env_path_info underflow - CVE-2019-11043
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
var_dump(\$_SERVER["PATH_INFO"]);
echo "Test End\n";
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$uri = $tester->makeSourceFile();
$tester
    ->request(
        '',
        [
            'SCRIPT_FILENAME' => $uri . "/" . str_repeat('A', 35),
            'PATH_INFO'       => '',
            'HTTP_HUI'        => str_repeat('PTEST', 1000),
        ],
        $uri
    )
    ->expectBody(
        [
            'Test Start',
            'string(0) ""',
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
