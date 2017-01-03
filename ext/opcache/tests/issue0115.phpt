--TEST--
ISSUE #115 (path issue when using phar)
--INI--
opcache.enable=1
opcache.enable_cli=1
phar.readonly=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (php_sapi_name() != "cli") die("skip CLI only"); ?>
--FILE--
<?php
$stub = '<?php
Phar::interceptFileFuncs();
require "phar://this/index.php";
__HALT_COMPILER(); ?>';
$p = new Phar(__DIR__ . '/issue0115_1.phar.php', 0, 'this');
$p['index.php'] = '<?php
  echo "Hello from Index 1.\n";
  require_once "phar://this/hello.php";
';
$p['hello.php'] = "Hello World 1!\n";
$p->setStub($stub);
unset($p);
$p = new Phar(__DIR__ . '/issue0115_2.phar.php', 0, 'this');
$p['index.php'] = '<?php
  echo "Hello from Index 2.\n";
  require_once "phar://this/hello.php";
';
$p['hello.php'] = "Hello World 2!\n";
$p->setStub($stub);
unset($p);

include "php_cli_server.inc";
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d extension=phar.'.PHP_SHLIB_SUFFIX);
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/issue0115_1.phar.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/issue0115_2.phar.php');
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/issue0115_1.phar.php');
@unlink(__DIR__ . '/issue0115_2.phar.php');
?>
--EXPECT--
Hello from Index 1.
Hello World 1!
Hello from Index 2.
Hello World 2!
