--TEST--
new mysqli()
--SKIPIF--
<?php
	require_once('skipif.inc');
	require_once('skipifemb.inc');
	require_once('skipifconnectfailure.inc');
	if (!get_current_user())
		die('skip: get_current_user() not supported');
	if (stristr(mysqli_get_client_info(), 'mysqlnd'))
		die("skip: test for libmysql (different error output when using php streams");
?>
--FILE--
<?php
	require_once("connect.inc");

	$myhost = 'invalidhost';
	$link   = NULL;

	print "1) bail\n";
	if (!is_object($mysqli = new mysqli($myhost)) || ('mysqli' !== get_class($mysqli)))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($mysqli), (is_object($mysqli)) ? var_export($mysqli, true) : $mysqli);

	print "2) be quiet\n";
	if (!is_object($mysqli = @new mysqli($myhost)) || ('mysqli' !== get_class($mysqli)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($mysqli), (is_object($mysqli)) ? var_export($mysqli, true) : $mysqli);
	var_dump(mysqli_connect_error());
	var_dump(mysqli_connect_errno());

	print "3) bail\n";
	if (false !== ($link = mysqli_connect($myhost))) {
		printf("[003] Expecting boolean/false, got %s/%s\n", gettype($link), $link);
	}

	print "4) be quiet\n";
	if (false !== ($link = @mysqli_connect($myhost))) {
		printf("[004] Expecting boolean/false, got %s/%s\n", gettype($link), $link);
	}
	var_dump(mysqli_connect_error());
	var_dump(mysqli_connect_errno());

	print "done!";
?>
--EXPECTF--
1) bail

Warning: mysqli::__construct(): (HY000/200%d): %s
2) be quiet
%s(%d) "%s"
int(200%d)
3) bail

Warning: mysqli_connect(): (HY000/200%d): %s
4) be quiet
%s(%d) "%s"
int(200%d)
done!
