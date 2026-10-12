--TEST--
FPM: user.ini is loaded when the script directory prefixes doc_root
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$workDir = __DIR__ . '/user-ini-prefix';
@mkdir($workDir);
file_put_contents($workDir . '/info.php', '<?php echo ini_get("memory_limit"); ');
file_put_contents($workDir . '/.user.ini', "memory_limit=77M\n");

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

$tester = new FPM\Tester($cfg, '');
$tester->start();
$tester->expectLogStartNotices();
$tester
    ->request(scriptFilename: $workDir . '/info.php', headers: ['DOCUMENT_ROOT' => $workDir . '/www'])
    ->expectBody('77M');
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
@unlink(__DIR__ . '/user-ini-prefix/info.php');
@unlink(__DIR__ . '/user-ini-prefix/.user.ini');
@rmdir(__DIR__ . '/user-ini-prefix');
?>
