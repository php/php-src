--TEST--
enable local infile
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--INI--
mysqli.allow_local_infile=1
--FILE--
<?php
require_once("connect.inc");

$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$res = mysqli_query($link, 'SHOW VARIABLES LIKE "local_infile"');
$row = mysqli_fetch_assoc($res);
echo "server: ", $row['Value'], "\n";
mysqli_free_result($res);
mysqli_close($link);

echo "connector: ", ini_get("mysqli.allow_local_infile"), "\n";

print "done!\n";
?>
--EXPECTF--
server: %s
connector: 1
done!
