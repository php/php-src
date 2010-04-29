--TEST--
mysqli_options()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

/*
TODO: ext/mysqli might lack support for those options which are available
with the libmysql C call mysql_options(). Not sure which of them make
sense to have in PHP and not even sure which of them might be available
already through other measures.

	MYSQL_OPT_COMPRESS (argument: not used) --> Andrey/Ulf: bug, should be added
	? MYSQL_OPT_NAMED_PIPE (argument: not used) ?
	MYSQL_OPT_READ_TIMEOUT (argument type: unsigned int *) --> Andrey/Ulf: bug, should be added
	MYSQL_OPT_RECONNECT (argument type: my_bool *) -->  Andrey/Ulf: might be security risk to have
	MYSQL_OPT_SSL_VERIFY_SERVER_CERT (argument type: my_bool *) --> Andrey/Ulf: might be security risk to have
	MYSQL_OPT_WRITE_TIMEOUT (argument type: unsigned int *) --> Andrey/Ulf: bug, should be added
	MYSQL_REPORT_DATA_TRUNCATION (argument type: my_bool *) --> Andrey: bug, although truncation might only happen with libmysql not with mysqlnd
	MYSQL_SECURE_AUTH (argument type: my_bool *) --> Ulf: let's say deprecated, no bug
	? MYSQL_SET_CHARSET_DIR (argument type: char *) ?
	MYSQL_SHARED_MEMORY_BASE_NAME (argument type: char *)
*/

	$valid_options = array(  MYSQLI_READ_DEFAULT_GROUP, MYSQLI_READ_DEFAULT_FILE,
		MYSQLI_OPT_CONNECT_TIMEOUT, MYSQLI_OPT_LOCAL_INFILE,
		MYSQLI_INIT_COMMAND, MYSQLI_READ_DEFAULT_GROUP,
		MYSQLI_READ_DEFAULT_FILE, MYSQLI_OPT_CONNECT_TIMEOUT,
		MYSQLI_OPT_LOCAL_INFILE, MYSQLI_INIT_COMMAND,
		MYSQLI_SET_CHARSET_NAME);

	if ($IS_MYSQLND && defined('MYSQLI_OPT_NET_CMD_BUFFER_SIZE'))
		$valid_options[] = constant('MYSQLI_OPT_NET_CMD_BUFFER_SIZE');
	if ($IS_MYSQLND && defined('MYSQLI_OPT_NET_READ_BUFFER_SIZE'))
		$valid_options[] = constant('MYSQLI_OPT_NET_READ_BUFFER_SIZE');
	if ($IS_MYSQLND && defined('MYSQLI_OPT_INT_AND_FLOAT_NATIVE'))
		$valid_options[] = constant('MYSQLI_OPT_INT_AND_FLOAT_NATIVE');

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_options()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_options($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	$link = mysqli_init();

	/* set it twice, checking if memory for the previous one is correctly freed */
	mysqli_options($link, MYSQLI_SET_CHARSET_NAME, "utf8");
	mysqli_options($link, MYSQLI_SET_CHARSET_NAME, "latin1");

	if (!is_null($tmp = @mysqli_options($link, MYSQLI_OPT_CONNECT_TIMEOUT)))
		printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_options($link, "s", 'extra_my.cnf')))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_options($link, MYSQLI_INIT_COMMAND, 'SET AUTOCOMMIT=0', 'foo')))
		printf("[005] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	// print "run_tests.php don't fool me with your 'ungreedy' expression '.+?'!\n";
	var_dump("MYSQLI_READ_DEFAULT_GROUP",	mysqli_options($link, MYSQLI_READ_DEFAULT_GROUP, 'extra_my.cnf'));
	var_dump("MYSQLI_READ_DEFAULT_FILE",	mysqli_options($link, MYSQLI_READ_DEFAULT_FILE, 'extra_my.cnf'));
	var_dump("MYSQLI_OPT_CONNECT_TIMEOUT",	mysqli_options($link, MYSQLI_OPT_CONNECT_TIMEOUT, 10));
	var_dump("MYSQLI_OPT_LOCAL_INFILE",		mysqli_options($link, MYSQLI_OPT_LOCAL_INFILE, 1));
	var_dump("MYSQLI_INIT_COMMAND",			mysqli_options($link, MYSQLI_INIT_COMMAND, array('SET AUTOCOMMIT=0', 'SET AUTOCOMMIT=1')));
	var_dump("MYSQLI_READ_DEFAULT_GROUP",	mysqli_options($link, MYSQLI_READ_DEFAULT_GROUP, 'extra_my.cnf'));
	var_dump("MYSQLI_READ_DEFAULT_FILE",	mysqli_options($link, MYSQLI_READ_DEFAULT_FILE, 'extra_my.cnf'));
	var_dump("MYSQLI_OPT_CONNECT_TIMEOUT",	mysqli_options($link, MYSQLI_OPT_CONNECT_TIMEOUT, 10));
	var_dump("MYSQLI_OPT_LOCAL_INFILE",		mysqli_options($link, MYSQLI_OPT_LOCAL_INFILE, 1));
	var_dump("MYSQLI_INIT_COMMAND",			mysqli_options($link, MYSQLI_INIT_COMMAND, 'SET AUTOCOMMIT=0'));
	var_dump("MYSQLI_CLIENT_SSL",			mysqli_options($link, MYSQLI_CLIENT_SSL, 'not an mysqli_option'));

	if ($IS_MYSQLND && defined('MYSQLI_OPT_INT_AND_YEARS_AS_INT') &&
		!($tmp = mysqli_options($link, constant('MYSQLI_OPT_INT_AND_YEARS_AS_INT'), true)))
		printf("[006] Expecting boolean/true got %s/%s\n", gettype($tmp), $tmp);

	if ($IS_MYSQLND) {
		/* Don't do this with libmysql. You may hit options not exported to PHP and cause false positives */
		for ($flag = -10000; $flag < 10000; $flag++) {
			if (in_array($flag, $valid_options))
				continue;
			if (FALSE !== ($tmp = mysqli_options($link, $flag, 'definetely not an mysqli_option'))) {
				var_dump(array("SOME_FLAG" => $flag, "ret" => $tmp));
			}
		}
	}

	mysqli_close($link);

	echo "Link closed";
	var_dump("MYSQLI_INIT_COMMAND", mysqli_options($link, MYSQLI_INIT_COMMAND, 'SET AUTOCOMMIT=1'));
	print "done!";
?>
--EXPECTF--
%s(25) "MYSQLI_READ_DEFAULT_GROUP"
bool(true)
%s(24) "MYSQLI_READ_DEFAULT_FILE"
bool(true)
%s(26) "MYSQLI_OPT_CONNECT_TIMEOUT"
bool(true)
%s(23) "MYSQLI_OPT_LOCAL_INFILE"
bool(true)

Notice: Array to string conversion in %s on line %d
%s(19) "MYSQLI_INIT_COMMAND"
bool(true)
%s(25) "MYSQLI_READ_DEFAULT_GROUP"
bool(true)
%s(24) "MYSQLI_READ_DEFAULT_FILE"
bool(true)
%s(26) "MYSQLI_OPT_CONNECT_TIMEOUT"
bool(true)
%s(23) "MYSQLI_OPT_LOCAL_INFILE"
bool(true)
%s(19) "MYSQLI_INIT_COMMAND"
bool(true)
%s(17) "MYSQLI_CLIENT_SSL"
bool(false)
Link closed
Warning: mysqli_options(): Couldn't fetch mysqli in %s line %d
%s(19) "MYSQLI_INIT_COMMAND"
NULL
done!
