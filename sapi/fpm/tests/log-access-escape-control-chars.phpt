--TEST--
FPM: access log escapes the control characters of request values
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$src = <<<EOT
<?php
echo "OK";
EOT;

$cfg = <<<EOT
[global]
error_log = {{RFILE:LOG:ERR}}
pid = {{RFILE:PID}}
[unconfined]
listen = {{ADDR}}
access.log = {{RFILE:LOG:ACC}}
access.format = "%u '%m %r%Q%q' %{HTTP_X_TEST}e %s"
pm = static
pm.max_children = 1
EOT;

$prefix = __DIR__;
$tester = new FPM\Tester($cfg, $src);
$tester->start(['--prefix', $prefix]);
$tester->expectLogStartNotices();
$tester->request(
    query: "a=1\r\nforged",
    headers: [
        'HTTP_AUTHORIZATION' => 'Basic ' . base64_encode("user\n127.0.0.1 - admin:pass"),
        'HTTP_X_TEST' => "x\x1by\x7f",
    ],
)->expectBody('OK');
$tester->expectAccessLog(
    "user\\x0a127.0.0.1 - admin 'GET /log-access-escape-control-chars.src.php?a=1\\x0d\\x0aforged' x\\x1by\\x7f 200"
);
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
$tester->checkAccessLog();

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
