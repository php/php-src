--TEST--
FPM: bug75712 - getenv should not read from $_ENV and $_SERVER
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
pm = static
pm.max_children = 1
env[TEST] = test
php_value[register_argc_argv] = on
EOT;

$code = <<<EOT
<?php

var_dump(isset(getenv()['argv']));
var_dump(isset(getenv()['SERVER_NAME']));
var_dump(getenv()['TEST']);
var_dump(isset(getenv()['DTEST']));
var_dump(getenv('DTEST'));
putenv('DTEST=dt');
var_dump(getenv()['DTEST']);
var_dump(getenv('DTEST'));

function notcalled()
{
    \$_SERVER['argv'];
}
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectBody([
    'bool(false)',
    'bool(true)',
    'string(4) "test"',
    'bool(false)',
    'bool(false)',
    'string(2) "dt"',
    'string(2) "dt"',
]);
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
