--TEST--
mysqli_free_resource() - resets the zval to NULL
--SKIPIF--
<?php
	require_once('skipif.inc');
	require_once('skipifemb.inc');
	require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require('connect.inc');
	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		die();
	}

	if (!($res = mysqli_query($link, "SELECT 1"))) {
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		die();
	}

	$row = mysqli_fetch_row($res);
	var_dump($row);
	var_dump($res);
	
	mysqli_free_result($res);
	
	var_dump($row);
	var_dump($res);
?>
--EXPECTF--
array(1) {
  [0]=>
  string(1) "1"
}
object(mysqli_result)#3 (5) {
  ["current_field"]=>
  int(0)
  ["field_count"]=>
  int(1)
  ["lengths"]=>
  array(1) {
    [0]=>
    int(1)
  }
  ["num_rows"]=>
  int(1)
  ["type"]=>
  int(0)
}
array(1) {
  [0]=>
  string(1) "1"
}
NULL
