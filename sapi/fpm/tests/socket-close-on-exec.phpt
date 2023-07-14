--TEST--
FPM: Set CLOEXEC on the listen and connection socket
--SKIPIF--
<?php
require_once "skipif.inc";
FPM\Tester::skipIfShellCommandFails('lsof -v 2> /dev/null');
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<'EOT'
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 1
pm.status_listen = {{ADDR[status]}}
pm.status_path = /status
env[PATH] = $PATH
EOT;

$code = <<<'EOT'
<?php

$fpmPort = $_GET['fpm_port'];

echo "My sockets (expect to see 2 of them - one ESTABLISHED and one LISTEN):\n";

$mypid = getmypid();
$ph = popen("/bin/sh -c 'lsof -Pn -p$mypid' 2>&1 | grep TCP | grep '127.0.0.1:$fpmPort'", 'r');
echo stream_get_contents($ph);
pclose($ph);

echo "\n";

/*
We expect that both LISTEN (inherited from the master process) and ACCEPTED (ESTABLISHED)
have F_CLOEXEC and should not appear in the created process.

We grep out sockets from non-FPM port, because they may appear in the environment,
e.g. PHP-FPM can inherit them from the test-runner. We cannot control the runner,
but we can test how the FPM behaves.
*/

echo "Sockets after exec(), expected to be empty:\n";
$ph = popen("/bin/sh -c 'lsof -Pn -p\$\$' 2>&1 | grep TCP | grep '127.0.0.1:$fpmPort'", 'r');
var_dump(stream_get_contents($ph));
pclose($ph);

EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request(query: 'fpm_port='.$tester->getPort())->printBody();
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECTF--
My sockets (expect to see 2 of them - one ESTABLISHED and one LISTEN):
%S 127.0.0.1:%d->127.0.0.1:%d (ESTABLISHED)
%S 127.0.0.1:%d (LISTEN)

Sockets after exec(), expected to be empty:
string(0) ""
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
