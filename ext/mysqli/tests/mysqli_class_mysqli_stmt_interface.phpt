--TEST--
Interface of the class mysqli_stmt
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

	$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
	$stmt = new mysqli_stmt($link);

	printf("Parent class:\n");
	var_dump(get_parent_class($stmt));

	printf("\nMethods:\n");

	$methods = get_class_methods($stmt);
	$expected_methods = array(
		'__construct'       => true,
		'attr_get'          => true,
		'attr_set'          => true,
		'bind_param'        => true,
		'bind_result'       => true,
		'close'             => true,
		'data_seek'         => true,
		'execute'           => true,
		'fetch'             => true,
		'free_result'       => true,
		'get_warnings'      => true,
		'num_rows'          => true,
		'prepare'           => true,
		'reset'             => true,
		'result_metadata'   => true,
		'send_long_data'    => true,
		'store_result'      => true,
	);

	if ($IS_MYSQLND) {
		$expected_methods['get_result'] = true;
		$expected_methods['more_results'] = true;
		$expected_methods['next_result'] = true;
	}

	foreach ($methods as $k => $method) {
	if (isset($expected_methods[$method])) {
		unset($methods[$k]);
		unset($expected_methods[$method]);
	}
		if ($method == 'mysqli_stmt') {
			// get_class_method reports different constructor names
			unset($expected_methods['__construct']);
			unset($methods[$k]);
		}
	}
	if (!empty($methods)) {
		printf("More methods found than indicated. Dumping list of unexpected methods.\n");
		var_dump($methods);
	}
	if (!empty($expected_methods)) {
		printf("Some methods are missing. Dumping list of missing methods.\n");
		var_dump($expected_methods);
	}
	if (empty($methods) && empty($expected_methods))
		printf("ok\n");

	printf("\nClass variables:\n");
	$variables = array_keys(get_class_vars(get_class($stmt)));
	sort($variables);
	foreach ($variables as $k => $var)
		printf("%s\n", $var);

	printf("\nObject variables:\n");
	$variables = array_keys(get_object_vars($stmt));
	foreach ($variables as $k => $var)
		printf("%s\n", $var);

printf("\nMagic, magic properties:\n");

assert(mysqli_stmt_affected_rows($stmt) === $stmt->affected_rows);
printf("stmt->affected_rows = '%s'\n", $stmt->affected_rows);

if (!$stmt->prepare("INSERT INTO test(id, label) VALUES (100, 'z')") ||
!$stmt->execute())
printf("[001] [%d] %s\n", $stmt->errno, $stmt->error);

assert(mysqli_stmt_affected_rows($stmt) === $stmt->affected_rows);
printf("stmt->affected_rows = '%s'\n", $stmt->affected_rows);

assert(mysqli_stmt_errno($stmt) === $stmt->errno);
printf("stmt->errno = '%s'\n", $stmt->errno);

assert(mysqli_stmt_error($stmt) === $stmt->error);
printf("stmt->error = '%s'\n", $stmt->error);

assert(mysqli_stmt_error_list($stmt) === $stmt->error_list);
var_dump("stmt->error = ", $stmt->error_list);

assert(mysqli_stmt_field_count($stmt) === $stmt->field_count);
printf("stmt->field_count = '%s'\n", $stmt->field_count);

assert($stmt->id > 0);
printf("stmt->id = '%s'\n", $stmt->id);

assert(mysqli_stmt_insert_id($stmt) === $stmt->insert_id);
printf("stmt->insert_id = '%s'\n", $stmt->insert_id);

assert(mysqli_stmt_num_rows($stmt) === $stmt->num_rows);
printf("stmt->num_rows = '%s'\n", $stmt->num_rows);

assert(mysqli_stmt_param_count($stmt) === $stmt->param_count);
printf("stmt->param_count = '%s'\n", $stmt->param_count);

assert(mysqli_stmt_sqlstate($stmt) === $stmt->sqlstate);
printf("stmt->sqlstate = '%s'\n", $stmt->sqlstate);

printf("\nAccess to undefined properties:\n");
printf("stmt->unknown = '%s'\n", @$stmt->unknown);
@$stmt->unknown = 13;
printf("stmt->unknown = '%s'\n", @$stmt->unknown);

printf("\nPrepare using the constructor:\n");
$stmt = new mysqli_stmt($link, 'SELECT id FROM test ORDER BY id');
if (!$stmt->execute())
printf("[002] [%d] %s\n", $stmt->errno, $stmt->error);
$stmt->close();

$obj = new stdClass();
if (!is_object($stmt = new mysqli_stmt($link, $obj)))
printf("[003] Expecting NULL got %s/%s\n", gettype($stmt), $stmt);

print "done!";
?>
--EXPECTF--
Parent class:
bool(false)

Methods:
ok

Class variables:
affected_rows
errno
error
error_list
field_count
id
insert_id
num_rows
param_count
sqlstate

Object variables:
affected_rows
insert_id
num_rows
param_count
field_count
errno
error
error_list
sqlstate
id

Magic, magic properties:

Warning: mysqli_stmt_affected_rows(): invalid object or resource mysqli_stmt
 in %s on line %d

Warning: main(): Property access is not allowed yet in %s on line %d

Warning: main(): Property access is not allowed yet in %s on line %d
stmt->affected_rows = ''
stmt->affected_rows = '1'
stmt->errno = '0'
stmt->error = ''
string(14) "stmt->error = "
array(0) {
}
stmt->field_count = '0'
stmt->id = '%d'
stmt->insert_id = '0'
stmt->num_rows = '0'
stmt->param_count = '0'
stmt->sqlstate = '00000'

Access to undefined properties:
stmt->unknown = ''
stmt->unknown = '13'

Prepare using the constructor:

Warning: mysqli_stmt::__construct() expects parameter 2 to be %binary_string_optional%, object given in %s on line %d
done!