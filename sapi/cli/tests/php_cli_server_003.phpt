--TEST--
Bug #55726 (Changing the working directory makes router script inaccessible)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc"; 
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die ("skip not for Windows");
}
?>
--FILE--
<?php
include "php_cli_server.inc";
$php = getenv('TEST_PHP_EXECUTABLE');
$handle = php_cli_server_start($php, 'chdir("/tmp"); echo "okey";');
var_dump(file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS));
var_dump(file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS));
php_cli_server_shutdown($handle);
?>
--EXPECTF--	
string(4) "okey"
string(4) "okey"
