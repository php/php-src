--TEST--
Files are invalidated if the package definition changes (with file cache)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--CONFLICTS--
server
--INI--
opcache.file_cache={TMP}
opcache.file_cache_only=0
opcache.jit_buffer_size=0
--FILE--
<?php

include __DIR__ . "/php_cli_server.inc";
php_cli_server_start(getenv('TEST_PHP_EXTRA_ARGS'));

echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/packages_invalidation_with_strict_types.php" );
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/packages_invalidation_with_strict_types.php" );
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/packages_invalidation_without_strict_types.php" );
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/packages_invalidation_without_strict_types.php" );
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/packages_invalidation_with_strict_types.php" );
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/packages_invalidation_with_strict_types.php" );

?>
--EXPECTF--
strlen() expects parameter 1 to be string, int given
Misses: %d
strlen() expects parameter 1 to be string, int given
Misses: %d
int(1)
Misses: %d
int(1)
Misses: %d
strlen() expects parameter 1 to be string, int given
Misses: %d
strlen() expects parameter 1 to be string, int given
Misses: %d
