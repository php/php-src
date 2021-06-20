--TEST--
ensure default for local infile is off
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require_once("connect.inc");

$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$res = mysqli_query($link, 'SHOW VARIABLES LIKE "local_infile"');
$row = mysqli_fetch_assoc($res);
echo "server: ", $row['Value'], "\n";
mysqli_free_result($res);
mysqli_close($link);

echo 'connector: ', ini_get('mysqli.allow_local_infile'), ' ', var_export(ini_get('mysqli.local_infile_directory')), "\n";

print "done!\n";
?>
--EXPECTF--
server: %s
connector: 0 ''
done!
