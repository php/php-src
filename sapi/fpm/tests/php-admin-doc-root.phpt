--TEST--
FPM: php_admin_value doc_root usage
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$docRoot = __DIR__ . '/';

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
php_admin_value[doc_root] = $docRoot
EOT;

$code = <<<EOT
<?php
echo "OK";
EOT;

$tester = new FPM\Tester($cfg, $code);
$sourceFile = $tester->makeSourceFile();
$tester->start();
$tester->expectLogStartNotices();
$tester->request(uri: basename($sourceFile), scriptFilename: $sourceFile)->expectBody('OK');
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
