--TEST--
FPM: FastCGI env var path info fix for Apache balancer real configuration
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
php_admin_value[cgi.fix_pathinfo] = yes
EOT;

$code = <<<EOT
<?php
echo \$_SERVER["SCRIPT_NAME"] . "\n";
echo \$_SERVER["SCRIPT_FILENAME"] . "\n";
echo \$_SERVER["PATH_INFO"] . "\n";
echo \$_SERVER["PHP_SELF"];
EOT;

$tester = new FPM\Tester($cfg, $code);
[$sourceFilePath, $scriptName] = $tester->createSourceFileAndScriptName();
$pathTranslatedPart = __DIR__ . $sourceFilePath .  '/pinfo';
$tester->start();
$tester->expectLogStartNotices();
$tester
    ->request(
        headers: [
            'PATH_INFO' => $sourceFilePath . '/pinfo',
            'PATH_TRANSLATED' => 'proxy:balancer://myappcluster' . $pathTranslatedPart . $pathTranslatedPart,
        ],
        uri: $scriptName . '/pinfo',
        scriptFilename: $sourceFilePath .  '/pinfo',
        scriptName: '',
    )
    ->expectBody([$sourceFilePath, $sourceFilePath, '/pinfo', $sourceFilePath . '/pinfo']);
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
