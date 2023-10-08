--TEST--
FPM: FastCGI env var path info fix for Apache ProxyPass SCRIPT_NAME stripping with invalid path
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
echo \$_SERVER["ORIG_SCRIPT_NAME"] ?? 'none' . "\n";
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
        uri: $scriptName . '/pinfox',
        scriptFilename: "proxy:fcgi://" . $tester->getAddr() . $sourceFilePath . '/pinfoy',
        scriptName: $scriptName . '/pinfox' // this would be a bug in Apache - just for testing
    )
    ->expectBody([$scriptName . '/pinfox', 'none', $sourceFilePath, '/pinfoy', $scriptName . '/pinfox/pinfoy']);
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
