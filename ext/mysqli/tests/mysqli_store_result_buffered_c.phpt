--TEST--
mysqli_store_result()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
mysqlnd.debug=d:t:O,/tmp/mysqlnd.trace
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;


	require('table.inc');

	if (!$res = mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id"))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!is_object($res = mysqli_store_result($link, MYSQLI_STORE_RESULT_COPY_DATA)))
		printf("[004] Expecting object, got %s/%s. [%d] %s\n",
			gettype($res), $res, mysqli_errno($link), mysqli_error($link));

	if (true !== ($tmp = mysqli_data_seek($res, 2)))
		printf("[005] Expecting boolean/true, got %s/%s. [%d] %s\n",
			gettype($tmp), $tmp, mysqli_errno($link), mysqli_error($link));

	mysqli_free_result($res);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECT--
done!
