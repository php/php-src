--TEST--
Compressed protocol: packets on both sides of the compression threshold
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

$link = mysqli_init();
if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket, MYSQLI_CLIENT_COMPRESS)) {
    printf("[001] Cannot connect: [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
}
var_dump($link->query("SHOW SESSION STATUS LIKE 'Compression'")->fetch_row()[1]);

foreach ([1, 230, 235, 236, 237, 238, 240, 600, 4000, 100000, 3000000] as $length) {
    $value = substr(str_repeat('compressed protocol packet ', intdiv($length, 27) + 1), 0, $length);
    $row = $link->query("SELECT '$value' AS v, LENGTH('$value') AS l")->fetch_assoc();
    if ($row['v'] !== $value || (int) $row['l'] !== $length) {
        printf("[002] Round trip failed for %d bytes\n", $length);
    }
}

$stmt = $link->prepare("SELECT HEX(?)");
for ($i = 0; $i < 200; $i++) {
    $value = random_bytes(1 + $i * 37);
    $stmt->bind_param('s', $value);
    $stmt->execute();
    $stmt->bind_result($hex);
    $stmt->fetch();
    $stmt->free_result();
    if ($hex !== strtoupper(bin2hex($value))) {
        printf("[003] Round trip failed for %d bytes\n", strlen($value));
    }
}
$stmt->close();

$link->close();
print "done!";
?>
--EXPECT--
string(2) "ON"
done!
