--TEST--
FPM: GH-15335 - PHP_AUTH shutdown  use after free
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
log_level = notice
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 1
catch_workers_output = yes
php_admin_value[cgi.fix_pathinfo] = no
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
        headers: [ "HTTP_AUTHORIZATION" => "Basic Zm9vOg==", "REQUEST_METHOD" => "GET"],
        uri: $scriptName,
        address: '{{ADDR}}',
        scriptFilename: __DIR__ . "/__unknown.php",
        scriptName: "/",
    )
    ->expectStatus('404 Not Found');
$tester
    ->request(
        uri: $scriptName,
        address: '{{ADDR}}',
        params: [],
    );
$tester->expectNoLogPattern("/zend_mm_heap corrupted/");
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
