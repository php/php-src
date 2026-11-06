--TEST--
GH-24777 (Cached chunks from earlier requests cause OOM on a huge allocation)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
$a = [];
if (isset($_GET['heavy'])) {
    for ($i = 0; $i < 12000; $i++) {
        $a[] = str_repeat('x', 1000);
    }
} else {
    $s = str_repeat('y', 6 * 1024 * 1024);
    echo "OK\n";
}
PHP, 'index.php', ['-d', 'memory_limit=16M', '-d', 'html_errors=0']);

for ($i = 0; $i < 8; $i++) {
    file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/?heavy=1");
}
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/");
?>
--EXPECT--
OK
