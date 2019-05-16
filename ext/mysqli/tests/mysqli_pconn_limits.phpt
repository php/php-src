--TEST--
Persistent connections - limits (-1, unlimited)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
require_once("connect.inc");
?>
--INI--
mysqli.allow_persistent=1
mysqli.max_persistent=-1
mysqli.max_links=-1
--FILE--
<?php
	require_once("connect.inc");
	// opens a regular connection
	require_once("table.inc");

	if (!$res = mysqli_query($link, "SELECT 'works..' as _desc"))
		printf("[001] Cannot run query, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	$row = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	printf("Regular connection 1 - '%s'\n", $row['_desc']);

	if (!$link2 = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[002] Cannot open second regular connection, [%d] %s\n",
			mysqli_connect_errno(), mysqli_connect_error());

	if (!$res = mysqli_query($link2, "SELECT 'works...' as _desc"))
		printf("[003] Cannot run query, [%d] %s\n",
			mysqli_errno($link2), mysqli_error($link2));

	$row = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	printf("Regular connection 2 - '%s'\n", $row['_desc']);

	$host = 'p:' . $host;
	if (!$plink = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[004] Cannot create persistent connection using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s, [%d] %s\n",
			$host, $user, $db, $port, $socket,
			mysqli_connect_errno(), mysqli_connect_error());

	if (!$res = mysqli_query($plink, "SELECT 'works...' as _desc"))
		printf("[005] Cannot run query, [%d] %s\n",
			mysqli_errno($plink), mysqli_error($plink));

	$row = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	printf("Persistent connection 1 - '%s'\n", $row['_desc']);

	if (!$plink2 = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[006] Cannot create persistent connection using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s, [%d] %s\n",
			$host, $user, $db, $port, $socket,
			mysqli_connect_errno(), mysqli_connect_error());

	if (!$res = mysqli_query($plink2, "SELECT 'works...' as _desc"))
		printf("[007] Cannot run query, [%d] %s\n",
			mysqli_errno($plink2), mysqli_error($plink2));

	$row = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	printf("Persistent connection 2 - '%s'\n", $row['_desc']);

	$plink3 = mysqli_init();
	if (!my_mysqli_real_connect($plink3, $host, $user, $passwd, $db, $port, $socket))
		printf("[008] Cannot create persistent connection using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s, [%d] %s\n",
			$host, $user, $db, $port, $socket,
			mysqli_connect_errno(), mysqli_connect_error());

	if (!$res = mysqli_query($plink3, "SELECT 'works...' as _desc"))
		printf("[009] Cannot run query, [%d] %s\n",
			mysqli_errno($plink2), mysqli_error($plink2));

	$row = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	printf("Persistent connection 3 - '%s'\n", $row['_desc']);

	mysqli_close($link);
	mysqli_close($link2);
	mysqli_close($plink);
	mysqli_close($plink2);
	mysqli_close($plink3);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECT--
Regular connection 1 - 'works..'
Regular connection 2 - 'works...'
Persistent connection 1 - 'works...'
Persistent connection 2 - 'works...'
Persistent connection 3 - 'works...'
done!
