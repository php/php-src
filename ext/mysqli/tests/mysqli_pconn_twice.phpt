--TEST--
Calling connect() on an open persistent connection to create a new persistent connection
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
if (!$IS_MYSQLND)
	die("skip mysqlnd test only");

?>
--INI--
mysqli.allow_persistent=1
mysqli.max_persistent=-1
mysqli.max_links=-1
--FILE--
<?php
	require_once("connect.inc");

	$host = 'p:' . $host;
	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (true !== ($tmp = my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)))
		printf("[003] Expecting boolean/true got %s/%s\n", gettype($tmp), $tmp);

	/* it is undefined which pooled connection we get - thread ids may differ */

	if (!($res = mysqli_query($link, "SELECT 'ok' AS it_works")) ||
		!($row = mysqli_fetch_assoc($res)))
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	var_dump($row);
	mysqli_free_result($res);

	mysqli_close($link);

	if (!$link = new my_mysqli($host, $user, $passwd, $db, $port, $socket))
		printf("[007] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);


	if (true !== ($tmp = $link->real_connect($host, $user, $passwd, $db, $port, $socket)))
		printf("[009] Expecting boolean/true got %s/%s\n", gettype($tmp), $tmp);

	/* it is undefined which pooled connection we get - thread ids may differ */

	if (!($res = $link->query("SELECT 'works also with oo' AS syntax")) ||
			!($row = $res->fetch_assoc()))
		printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	var_dump($row);
	mysqli_free_result($res);

	mysqli_close($link);

	if (NULL !== ($tmp = $link->connect($host, $user, $passwd, $db, $port, $socket)))
		printf("[013] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[014] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (NULL !== ($tmp = $link->connect($host, $user, $passwd, $db, $port, $socket)))
		printf("[015] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--EXPECTF--
array(1) {
  [%u|b%"it_works"]=>
  %unicode|string%(2) "ok"
}
array(1) {
  [%u|b%"syntax"]=>
  %unicode|string%(18) "works also with oo"
}
done!