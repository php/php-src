--TEST--
Bug #78106: Path resolution fails if opcache disabled during request
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--CONFLICTS--
server
--FILE--
<?php

include __DIR__ . "/php_cli_server.inc";
php_cli_server_start(getenv('TEST_PHP_EXTRA_ARGS'));

echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/bug78106_test1.php" );
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/bug78106_test2.php" );
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/bug78106_test1.php" );

?>
--EXPECT--
included
done
included
done
included
done
