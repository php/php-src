--TEST--
PHP bug#79163: SQL SELECT EXISTS fills up memory:
--FILE--
<?php
function shutDownFunction()
{
	$error = error_get_last();

    if($error !== NULL)
		echo 'TEST FAIL: ' . $error["message"] . "\n";
}
register_shutdown_function('shutDownFunction');

ini_set('memory_limit', '2097152');

require_once("connect.inc");

$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

for ($i = 0; $i < 20000; $i++) {
	$sql = 'SELECT '.$i;
	$res = $mysqli->query($sql);
	unset($sql, $res);
}

$mysqli->close();

print "done!";
?>
--EXPECT--
done!
