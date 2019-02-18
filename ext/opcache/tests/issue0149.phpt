--TEST--
ISSUE #149 (Phar mount points not working this OPcache enabled)
--INI--
opcache.enable=1
opcache.enable_cli=1
phar.readonly=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (php_sapi_name() != "cli") die("skip CLI only"); ?>
--CONFLICTS--
server
--FILE--
<?php
$stub = "<?php header('Content-Type: text/plain;');
Phar::mount('this.file', '". __FILE__ . "');
echo 'OK\n';
__HALT_COMPILER(); ?>";
$p = new Phar(__DIR__ . '/issue0149.phar.php', 0, 'this');
$p['index.php'] = "";  # A Phar must have at least one file, hence this dummy
$p->setStub($stub);
unset($p);

include "php_cli_server.inc";
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d extension=phar.'.PHP_SHLIB_SUFFIX);
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/issue0149.phar.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/issue0149.phar.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/issue0149.phar.php');
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/issue0149.phar.php');
?>
--EXPECT--
OK
OK
OK
