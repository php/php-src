--TEST--
mysqli_stmt_bind_param used with call_user_func_array() (see also bug #43568)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require('connect.inc');
	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test WHERE id = ?'))
		printf("[002] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = 1;
	if (!mysqli_stmt_bind_param($stmt, 'i', $id) ||
		!mysqli_stmt_execute($stmt))
		printf("[003] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (!mysqli_stmt_bind_result($stmt, $id, $label) ||
		(true !== mysqli_stmt_fetch($stmt)))
		printf("[004] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	print "Regular, procedural, using variables\n";
	var_dump($id);
	var_dump($label);

	mysqli_stmt_close($stmt);
	if (!$stmt = mysqli_stmt_init($link))
		printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test WHERE id = ?'))
		printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$types = 'i';
	$id = 1;
	$params = array(
		0 => &$stmt,
		1 => &$types,
		2 => &$id
	);
	if (!call_user_func_array('mysqli_stmt_bind_param', $params))
		printf("[007] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (!mysqli_stmt_bind_result($stmt, $id, $label) ||
		(true !== mysqli_stmt_fetch($stmt)))
		printf("[009] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	print "Call user func, procedural, using references for everything\n";
	var_dump($id);
	var_dump($label);

	mysqli_stmt_close($stmt);
	if (!$stmt = mysqli_stmt_init($link))
		printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test WHERE id = ?'))
		printf("[011] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$types = 'i';
	$id = 1;
	$params = array(
		0 => &$types,
		1 => &$id
	);
	if (!call_user_func_array(array($stmt, 'bind_param'), $params))
		printf("[012] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[013] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (!mysqli_stmt_bind_result($stmt, $id, $label) ||
		(true !== mysqli_stmt_fetch($stmt)))
		printf("[014] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	print "Call user func, object oriented, using references for everything\n";
	var_dump($id);
	var_dump($label);

	mysqli_stmt_close($stmt);
	if (!$stmt = mysqli_stmt_init($link))
		printf("[015] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test WHERE id = ?'))
		printf("[016] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$types = 'i';
	$id = 1;
	$params = array(
		0 => $types,
		1 => &$id
	);
	if (!call_user_func_array(array($stmt, 'bind_param'), $params))
		printf("[017] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[018] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (!mysqli_stmt_bind_result($stmt, $id, $label) ||
		(true !== mysqli_stmt_fetch($stmt)))
		printf("[019] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	print "Call user func, object oriented, using variable for types. using references for bound parameter\n";
	var_dump($id);
	var_dump($label);

	mysqli_stmt_close($stmt);
	if (!$stmt = mysqli_stmt_init($link))
		printf("[020] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test WHERE id = ?'))
		printf("[021] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = 1;
	$params = array(
		0 => 'i',
		1 => &$id
	);
	if (!call_user_func_array(array($stmt, 'bind_param'), $params))
		printf("[022] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[023] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (!mysqli_stmt_bind_result($stmt, $id, $label) ||
		(true !== mysqli_stmt_fetch($stmt)))
		printf("[024] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	print "Call user func, object oriented, using constant for types. using references for bound parameter\n";
	var_dump($id);
	var_dump($label);

	mysqli_stmt_close($stmt);
	if (!$stmt = mysqli_stmt_init($link))
		printf("[025] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test WHERE id = ?'))
		printf("[026] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$types = 'i';
	$id = 1;
	$params = array(
		0 => &$stmt,
		1 => $types,
		2 => &$id
	);
	if (!call_user_func_array('mysqli_stmt_bind_param', $params))
		printf("[027] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[028] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (!mysqli_stmt_bind_result($stmt, $id, $label) ||
		(true !== mysqli_stmt_fetch($stmt)))
		printf("[029] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	print "Call user func, procedural, using references for everything but using variable for types\n";
	var_dump($id);
	var_dump($label);

	mysqli_stmt_close($stmt);
	if (!$stmt = mysqli_stmt_init($link))
		printf("[025] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test WHERE id = ?'))
		printf("[026] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$types = 'i';
	$id = 1;
	$params = array(
		0 => $stmt,
		1 => $types,
		2 => &$id
	);
	if (!call_user_func_array('mysqli_stmt_bind_param', $params))
		printf("[027] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[028] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (!mysqli_stmt_bind_result($stmt, $id, $label) ||
		(true !== mysqli_stmt_fetch($stmt)))
		printf("[029] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	print "Call user func, procedural, using references for bound parameter, using variables for resource and types\n";
	var_dump($id);
	var_dump($label);

	mysqli_stmt_close($stmt);
	if (!$stmt = mysqli_stmt_init($link))
		printf("[030] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test WHERE id = ?'))
		printf("[031] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$types = 'i';
	$id = 1;
	$params = array(
		0 => $stmt,
		1 => $types,
		2 => &$id
	);
	if (!call_user_func_array('mysqli_stmt_bind_param', $params))
		printf("[032] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[033] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (!mysqli_stmt_bind_result($stmt, $id, $label) ||
		(true !== mysqli_stmt_fetch($stmt)))
		printf("[034] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	print "Call user func, procedural, using references for bound parameter, using variables for resource and types\n";
	var_dump($id);
	var_dump($label);

	mysqli_stmt_close($stmt);
	if (!$stmt = mysqli_stmt_init($link))
		printf("[035] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test WHERE id = ?'))
		printf("[036] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = 1;
	$params = array(
		0 => $stmt,
		1 => 'i',
		2 => &$id
	);
	if (!call_user_func_array('mysqli_stmt_bind_param', $params))
		printf("[037] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[038] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (!mysqli_stmt_bind_result($stmt, $id, $label) ||
		(true !== mysqli_stmt_fetch($stmt)))
		printf("[039] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	print "Call user func, procedural, using references for bound parameter, using variable for resource, using constant for types\n";
	var_dump($id);
	var_dump($label);

	mysqli_stmt_close($stmt);
	if (!$stmt = mysqli_stmt_init($link))
		printf("[040] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test WHERE id = ?'))
		printf("[041] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = 1;
	if (!call_user_func_array('mysqli_stmt_bind_param', array($stmt, 'i', &$id)))
		printf("[042] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[043] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (!mysqli_stmt_bind_result($stmt, $id, $label) ||
		(true !== mysqli_stmt_fetch($stmt)))
		printf("[044] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	print "Call user func, procedural, using references for bound parameter, using variable for resource, using constant for types, array\n";
	var_dump($id);
	var_dump($label);

	//
	// Any of those shall fail - see also bugs.php.net/43568
	//
	mysqli_stmt_close($stmt);
	if (!$stmt = mysqli_stmt_init($link))
		printf("[045] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test WHERE id = ?'))
		printf("[046] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = 1;
	$params = array(
		0 => 'i',
		1 => &$id
	);
	if (!call_user_func_array(array($stmt, 'bind_param'), $params))
		printf("[047] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[048] [%d] (Message might vary with MySQL Server version, e.g. No data supplied for parameters in prepared statement)\n", mysqli_stmt_errno($stmt));

	mysqli_stmt_close($stmt);
	if (!$stmt = mysqli_stmt_init($link))
		printf("[049] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test WHERE id = ?'))
		printf("[050] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$types = 'i';
	$id = 1;
	$params = array(
		0 => $stmt,
		1 => 'i',
		2 => &$id
	);
	if (!call_user_func_array('mysqli_stmt_bind_param', $params))
		printf("[051] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[052] [%d] (Message might vary with MySQL Server version, e.g. No data supplied for parameters in prepared statement)\n", mysqli_stmt_errno($stmt));

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Regular, procedural, using variables
int(1)
string(1) "a"
Call user func, procedural, using references for everything
int(1)
string(1) "a"
Call user func, object oriented, using references for everything
int(1)
string(1) "a"
Call user func, object oriented, using variable for types. using references for bound parameter
int(1)
string(1) "a"
Call user func, object oriented, using constant for types. using references for bound parameter
int(1)
string(1) "a"
Call user func, procedural, using references for everything but using variable for types
int(1)
string(1) "a"
Call user func, procedural, using references for bound parameter, using variables for resource and types
int(1)
string(1) "a"
Call user func, procedural, using references for bound parameter, using variables for resource and types
int(1)
string(1) "a"
Call user func, procedural, using references for bound parameter, using variable for resource, using constant for types
int(1)
string(1) "a"
Call user func, procedural, using references for bound parameter, using variable for resource, using constant for types, array
int(1)
string(1) "a"
done!
