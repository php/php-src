--TEST--
References to result sets
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once('connect.inc');
	require_once('table.inc');

	$references = array();

	if (!(mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id ASC LIMIT 2")) ||
			!($res = mysqli_store_result($link)))
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$idx = 0;
	while ($row = mysqli_fetch_assoc($res)) {
		/* mysqlnd: force seperation - create copies */
		$references[$idx] = array(
			'id' 		=> &$row['id'],
			'label'	=> $row['label'] . '');
		$references[$idx++]['id'] += 0;
	}

	mysqli_close($link);

	mysqli_data_seek($res, 0);
	while ($row = mysqli_fetch_assoc($res)) {
		/* mysqlnd: force seperation - create copies */
		$references[$idx] = array(
			'id' 		=> &$row['id'],
			'label'	=> $row['label'] . '');
		$references[$idx++]['id'] += 0;
	}

	mysqli_free_result($res);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (!(mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id ASC LIMIT 2")) ||
			!($res = mysqli_use_result($link)))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	while ($row = mysqli_fetch_assoc($res)) {
		/* mysqlnd: force seperation - create copies*/
		$references[$idx] = array(
			'id' 		=> &$row['id'],
			'label'	=> $row['label'] . '');
		$references[$idx]['id2'] = &$references[$idx]['id'];
		$references[$idx]['id'] += 1;
		$references[$idx++]['id2'] += 1;
	}

	$references[$idx++] = &$res;
	mysqli_free_result($res);
	@debug_zval_dump($references);

	if (!(mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id ASC LIMIT 1")) ||
			!($res = mysqli_use_result($link)))
		printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$tmp = array();
	while ($row = mysqli_fetch_assoc($res)) {
		$tmp[] = $row;
	}
	$tmp = unserialize(serialize($tmp));
	debug_zval_dump($tmp);
	mysqli_free_result($res);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
array(7) refcount(2){
  [0]=>
  array(2) refcount(1){
    [%u|b%"id"]=>
    long(1) refcount(1)
    [%u|b%"label"]=>
    %unicode|string%(1) "a" refcount(1)
  }
  [1]=>
  array(2) refcount(1){
    [%u|b%"id"]=>
    long(2) refcount(1)
    [%u|b%"label"]=>
    %unicode|string%(1) "b" refcount(1)
  }
  [2]=>
  array(2) refcount(1){
    [%u|b%"id"]=>
    long(1) refcount(1)
    [%u|b%"label"]=>
    %unicode|string%(1) "a" refcount(1)
  }
  [3]=>
  array(2) refcount(1){
    [%u|b%"id"]=>
    long(2) refcount(1)
    [%u|b%"label"]=>
    %unicode|string%(1) "b" refcount(1)
  }
  [4]=>
  array(3) refcount(1){
    [%u|b%"id"]=>
    &long(3) refcount(2)
    [%u|b%"label"]=>
    %unicode|string%(1) "a" refcount(1)
    [%u|b%"id2"]=>
    &long(3) refcount(2)
  }
  [5]=>
  array(3) refcount(1){
    [%u|b%"id"]=>
    &long(4) refcount(2)
    [%u|b%"label"]=>
    %unicode|string%(1) "b" refcount(1)
    [%u|b%"id2"]=>
    &long(4) refcount(2)
  }
  [6]=>
  &object(mysqli_result)#2 (5) refcount(2){
    [%u|b%"current_field"]=>
    NULL refcount(1)
    [%u|b%"field_count"]=>
    NULL refcount(1)
    [%u|b%"lengths"]=>
    NULL refcount(1)
    [%u|b%"num_rows"]=>
    NULL refcount(1)
    [%u|b%"type"]=>
    NULL refcount(1)
  }
}
array(1) refcount(2){
  [0]=>
  array(2) refcount(1){
    [%u|b%"id"]=>
    %unicode|string%(1) "1" refcount(1)
    [%u|b%"label"]=>
    %unicode|string%(1) "a" refcount(1)
  }
}
done!