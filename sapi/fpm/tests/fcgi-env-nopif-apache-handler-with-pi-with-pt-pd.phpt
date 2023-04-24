--TEST--
FPM: FastCGI env var without path info fix for Apache handler with PATH_INFO, PATH_TRANSLATED and path discard
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
php_admin_value[cgi.fix_pathinfo] = no
php_admin_value[cgi.discard_path] = yes
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
$tester->start();
$tester->expectLogStartNotices();
$tester
    ->request(
        headers: [
            'PATH_INFO' => '/pinfo',
            'PATH_TRANSLATED' => __DIR__ . '/pinfo',
        ],
        uri: $scriptName . '/pinfo',
        scriptFilename: "proxy:fcgi://" . $tester->getAddr() . $sourceFilePath,
        scriptName: $scriptName,
    )
    ->expectBody([$scriptName, $sourceFilePath, '/pinfo', '/pinfo']);
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
