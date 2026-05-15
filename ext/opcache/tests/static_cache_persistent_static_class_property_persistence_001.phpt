--TEST--
OPcache PersistentStatic persists class static properties across requests
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/persistent_static_001.php', <<<'PHP'
<?php
#[OPcache\PersistentStatic]
class ClassCounter
{
    public static int $value = 0;
}

class PropertyCounter
{
    #[OPcache\PersistentStatic]
    public static int $value = 0;
}

class NormalCounter
{
    public static int $value = 0;
}

echo ++ClassCounter::$value, ',', ++PropertyCounter::$value, ',', ++NormalCounter::$value, "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
    $php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
    putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.persistent_size_mb=32');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_001.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_001.php');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/persistent_static_001.php');
?>
--EXPECT--
1,1,1
2,2,1
