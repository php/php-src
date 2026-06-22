--TEST--
OPcache StableStatic stays disabled when stable cache memory is 0
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/stable_static_008.php', <<<'PHP'
<?php
#[OPcache\StableStatic]
class DisabledCounter
{
    public static int $value = 0;
}

echo ++DisabledCounter::$value, "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
    $php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
    putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.stable_size_mb=0');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_008.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_008.php');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stable_static_008.php');
?>
--EXPECT--
1
1
