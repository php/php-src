--TEST--
Files are invalidated if the package definition changes
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--CONFLICTS--
server
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
--EXPECT--
strlen() expects parameter 1 to be string, int given
Misses: 2
strlen() expects parameter 1 to be string, int given
Misses: 2
int(1)
Misses: 4
int(1)
Misses: 4
strlen() expects parameter 1 to be string, int given
Misses: 5
strlen() expects parameter 1 to be string, int given
Misses: 5
