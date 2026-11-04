--TEST--
MYSQLI_CLIENT_COMPRESS compresses the data in both directions
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
ob_start();
(new ReflectionExtension('mysqlnd'))->info();
if (!str_contains(ob_get_clean(), 'Compression => supported')) {
    die('skip mysqlnd is built without compression support');
}
?>
--FILE--
<?php
require_once 'connect.inc';

function server_bytes(mysqli $link): array
{
    $result = $link->query("SHOW SESSION STATUS WHERE Variable_name IN ('Bytes_received', 'Bytes_sent')");
    return array_map(intval(...), array_column($result->fetch_all(), 1, 0));
}

$size = 100000;

foreach ([0 => 'Uncompressed', MYSQLI_CLIENT_COMPRESS => 'Compressed'] as $flags => $label) {
    $link = mysqli_init();
    if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket, $flags, false)) {
        printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
        continue;
    }

    $before = server_bytes($link);
    $link->query("DO '" . str_repeat('x', $size) . "'");
    $after_send = server_bytes($link);
    $row = $link->query("SELECT REPEAT('x', $size)")->fetch_row();
    $after_receive = server_bytes($link);

    $client_to_server = $after_send['Bytes_received'] - $before['Bytes_received'];
    $server_to_client = $after_receive['Bytes_sent'] - $after_send['Bytes_sent'];

    echo "$label:\n";
    printf("  client to server is compressed: %s\n", $client_to_server < $size / 10 ? 'yes' : 'no');
    printf("  server to client is compressed: %s\n", $server_to_client < $size / 10 ? 'yes' : 'no');
    printf("  result is correct: %s\n", $row[0] === str_repeat('x', $size) ? 'yes' : 'no');
}
?>
--EXPECT--
Uncompressed:
  client to server is compressed: no
  server to client is compressed: no
  result is correct: yes
Compressed:
  client to server is compressed: yes
  server to client is compressed: yes
  result is correct: yes
