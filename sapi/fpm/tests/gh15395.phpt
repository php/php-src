--TEST--
GH-15335 data logging during shutdown
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR:UDS}}
pm = dynamic
pm.max_children = 2
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 2
php_admin_value[cgi.fix_pathinfo] = yes
EOT;

$code = <<<EOT
<?php
echo \$_SERVER["SCRIPT_NAME"] . "\n";
echo \$_SERVER["SCRIPT_FILENAME"] . "\n";
echo \$_SERVER["PHP_SELF"];
EOT;

$tester = new FPM\Tester($cfg, $code);
[$sourceFilePath, $scriptName] = $tester->createSourceFileAndScriptName();
$tester->start();
$tester->expectLogStartNotices();
$tester
    ->request(
        uri: $scriptName,
        address: '{{ADDR:UDS}}',
        scriptFilename: "/",
        scriptName: "/",
	httpAuthorization: "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==",
    );

$tester
    ->request(
        uri: $scriptName,
        address: '{{ADDR:UDS}}',
        scriptFilename: "/",
        scriptName: "/",
    );
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

