--TEST--
mysqli_stmt_execute() - SP, next result
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
	die(sprintf('skip Cannot connect to MySQL, [%d] %s.', mysqli_connect_errno(), mysqli_connect_error()));
}
if (mysqli_get_server_version($link) < 50503) {
	die(sprintf('skip Needs MySQL 5.5.3+, found version %d.', mysqli_get_server_version($link)));
}
?>
--FILE--
<?php
	require_once('connect.inc');

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		  $host, $user, $db, $port, $socket);
	}

	if (!mysqli_query($link, 'DROP PROCEDURE IF EXISTS p'))
		printf("[003] [%d] %s.\n", mysqli_errno($link), mysqli_error($link));

	if (mysqli_real_query($link, 'CREATE PROCEDURE p(IN ver_in VARCHAR(25)) BEGIN SELECT ver_in AS _ver_out; END;')) {
		// one result set
		if (!$stmt = mysqli_prepare($link, 'CALL p(?)'))
			printf("[005] Cannot prepare CALL, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

		$version = 'myversion';
		if (!mysqli_stmt_bind_param($stmt, 's', $version))
			printf("[006] Cannot bind input parameter, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		if (!mysqli_stmt_execute($stmt))
			printf("[007] Cannot execute CALL, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		$version = 'unknown';
		if (!mysqli_stmt_bind_result($stmt, $version) ||
				!mysqli_stmt_fetch($stmt))
			printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		if ($version !== "myversion")
			printf("[009] Results seem wrong, got %s, [%d] %s\n",
				$version,
				mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		mysqli_stmt_free_result($stmt);

		printf("[010] More results: %s\n", (mysqli_more_results($link)) ? "yes" : "no");
		printf("[011] Next result: %s\n", (mysqli_next_result($link)) ? "yes" : "no");

		if (!mysqli_stmt_close($stmt))
			printf("[012] Cannot close statement, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		if (!$link->query("SELECT 1"))
			printf("[013] [%d] %s\n", $link->errno, $link->error);

	} else {
		printf("[004] Cannot create SP, [%d] %s.\n", mysqli_errno($link), mysqli_error($link));
	}

	if (!mysqli_query($link, 'DROP PROCEDURE IF EXISTS p'))
		printf("[014] [%d] %s.\n", mysqli_errno($link), mysqli_error($link));

	if (mysqli_real_query($link, 'CREATE PROCEDURE p(IN ver_in VARCHAR(25)) BEGIN SELECT ver_in AS _ver_out; SELECT 1 AS _more; END;')) {
		// two result sets
		if (!$stmt = mysqli_prepare($link, 'CALL p(?)'))
			printf("[015] Cannot prepare CALL, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

		$version = 'myversion';
		if (!mysqli_stmt_bind_param($stmt, 's', $version))
			printf("[016] Cannot bind input parameter, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		if (!mysqli_stmt_execute($stmt))
			printf("[017] Cannot execute CALL, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		$version = NULL;
		if (!mysqli_stmt_bind_result($stmt, $version) ||
				!mysqli_stmt_fetch($stmt))
			printf("[018] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		if ($version !== "myversion")
			printf("[019] Results seem wrong, got %s, [%d] %s\n",
				$version,
				mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		if (!mysqli_more_results($link) || !mysqli_next_result($link))
			printf("[020] [%d] %s\n", $link->errno, $link->error);

		$more = NULL;
		if (!mysqli_stmt_bind_result($stmt, $more) ||
				!mysqli_stmt_fetch($stmt))
			printf("[021] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		if ($more !== 1)
			printf("[022] Results seem wrong, got %s, [%d] %s\n",
				$more,
				mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		if (!mysqli_stmt_close($stmt))
			printf("[023] Cannot close statement, [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));


		if (!$link->query("SELECT 1"))
			printf("[024] [%d] %s\n", $link->errno, $link->error);

	} else {
		printf("[025] Cannot create SP, [%d] %s.\n", mysqli_errno($link), mysqli_error($link));
	}

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

@mysqli_query($link, 'DROP PROCEDURE IF EXISTS p');

mysqli_close($link);
?>
--XFAIL--
Unsupported and undefined, under development
--EXPECT--
[010] More results: yes
[011] Next result: yes
done!
