--TEST--
OPcache VolatileStatic Immediate publishes scalar assignments through tracked references
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/volatile_cache_tracked_reference_assignment_001.php', <<<'PHP'
<?php
class VolatileCacheTrackedReferenceAssignment
{
    #[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Immediate)]
    public static function &value(): mixed
    {
        static $value = null;

        return $value;
    }
}

class VolatileCacheTrackedReferenceAssignmentTyped
{
    public static string $value = '';
}

$action = $_GET['action'] ?? 'read';

if ($action === 'reset') {
    opcache_static_cache_volatile_reset();
    opcache_reset();
    echo "reset\n";
    return;
}

if ($action === 'seed') {
    $alias =& VolatileCacheTrackedReferenceAssignment::value();
    $alias = 'one';
    echo 'seed=', VolatileCacheTrackedReferenceAssignment::value(), "\n";
    return;
}

if ($action === 'mutate') {
    $alias =& VolatileCacheTrackedReferenceAssignment::value();
    VolatileCacheTrackedReferenceAssignmentTyped::$value =& $alias;
    $alias = 'two';
    echo 'mutate=', VolatileCacheTrackedReferenceAssignment::value(), "\n";
    return;
}

echo 'read=', VolatileCacheTrackedReferenceAssignment::value() ?? 'null', "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
    $php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
    putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_cache_tracked_reference_assignment_001.php';
echo file_get_contents($base . '?action=reset');
echo file_get_contents($base . '?action=seed');
echo file_get_contents($base . '?action=read');
echo file_get_contents($base . '?action=mutate');
echo file_get_contents($base . '?action=read');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/volatile_cache_tracked_reference_assignment_001.php');
?>
--EXPECT--
reset
seed=one
read=one
mutate=two
read=two
