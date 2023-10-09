--TEST--
Bug GH-8846: Delayed early binding can be used for classes without parents
--EXTENSIONS--
opcache
--CONFLICTS--
server
--INI--
opcache.validate_timestamps=1
opcache.revalidate_freq=0
--FILE--
<?php

file_put_contents(__DIR__ . '/gh8846-index.php', <<<'PHP'
<?php
if (!@$_GET['skip']) {
    include __DIR__ . '/gh8846-1.inc';
}
include __DIR__ . '/gh8846-2.inc';
echo "Ok\n";
PHP);

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/gh8846-index.php');
echo "\n";
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/gh8846-index.php?skip=1');
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh8846-index.php');
?>
--EXPECTF--
bool(true)
<br />
<b>Fatal error</b>:  Cannot declare class Foo, because the name is already in use in <b>%sgh8846-2.inc</b> on line <b>%d</b><br />

bool(true)
Ok
