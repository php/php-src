--TEST--
OPcache PinnedStatic persists method static variables across requests
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/pinned_static_002.php', <<<'PHP'
<?php
function pinned_static_seed(string $logFile): int
{
    file_put_contents($logFile, "seed\n", FILE_APPEND);
    return 1;
}

#[OPcache\PinnedStatic]
class MethodCounter
{
    public static function constCounter(): int
    {
        static $value = 0;

        return ++$value;
    }

    public static function dynamicCounter(string $logFile): int
    {
        static $value = pinned_static_seed($logFile);

        return ++$value;
    }
}

class NormalMethodCounter
{
    public static function constCounter(): int
    {
        static $value = 0;

        return ++$value;
    }
}

$logFile = __DIR__ . '/pinned_static_002.log';
echo MethodCounter::constCounter(), ',', MethodCounter::dynamicCounter($logFile), ',', NormalMethodCounter::constCounter(), ',', count(file($logFile, FILE_IGNORE_NEW_LINES)), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
    $php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
    putenv('TEST_PHP_EXECUTABLE=' . $php);
}

@unlink(__DIR__ . '/pinned_static_002.log');

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.pinned_size_mb=32');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_002.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_002.php');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/pinned_static_002.php');
@unlink(__DIR__ . '/pinned_static_002.log');
?>
--EXPECT--
1,2,1,1
2,3,1,1
