--TEST--
Interface of the class mysqli_warning - TODO
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');

if (!$TEST_EXPERIMENTAL)
	die("skip - experimental (= unsupported) feature");
?>
--FILE--
<?php
	require('connect.inc');

	$warning = new mysqli_warning();
	$warning = new mysqli_warning(null);
	$warning = new mysqli_warning(null, null);

	$mysqli = new mysqli();
	$warning = new mysqli_warning($mysqli);

	$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
	$stmt = new mysqli_stmt($mysqli);
	$warning = new mysqli_warning($stmt);

	$stmt = $mysqli->stmt_init();
	$warning = new mysqli_warning($stmt);

	$obj = new stdClass();
	$warning = new mysqli_warning($obj);

	include("table.inc");
	$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
	$res = $mysqli->query('INSERT INTO test(id, label) VALUES (1, "zz")');
	$warning = mysqli_get_warnings($mysqli);

	printf("Parent class:\n");
	var_dump(get_parent_class($warning));

	printf("\nMethods:\n");
	$methods = get_class_methods($warning);
	$expected_methods = array(
		'next'                      => true,
	);

	foreach ($methods as $k => $method) {
		if (isset($expected_methods[$method])) {
			unset($methods[$k]);
			unset($expected_methods[$method]);
		}
	}
	if (!empty($methods)) {
		printf("Dumping list of unexpected methods.\n");
		var_dump($methods);
	}
	if (!empty($expected_methods)) {
		printf("Dumping list of missing methods.\n");
		var_dump($expected_methods);
	}
	if (empty($methods) && empty($expected_methods))
		printf("ok\n");

	printf("\nClass variables:\n");
	$variables = get_class_vars(get_class($mysqli));
	sort($variables);
	foreach ($variables as $k => $var)
		printf("%s\n", $var);

	printf("\nObject variables:\n");
	$variables = get_object_vars($mysqli);
	foreach ($variables as $k => $var)
		printf("%s\n", $var);

	printf("\nMagic, magic properties:\n");

	assert('' === $warning->message);
	printf("warning->message = '%s'\n", $warning->message);

	assert('' === $warning->sqlstate);
	printf("warning->sqlstate= '%s'\n", $warning->sqlstate);

	assert(0 === $warning->errno);
	printf("warning->errno = '%s'\n", $warning->errno);

	printf("\nAccess to undefined properties:\n");
	printf("warning->unknown = '%s'\n", @$warning->unknown);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: Wrong parameter count for mysqli_warning::mysqli_warning() in %s on line %d

Warning: mysqli_warning::mysqli_warning() expects parameter 1 to be object, null given in %s on line %d

Warning: Wrong parameter count for mysqli_warning::mysqli_warning() in %s on line %d

Warning: mysqli_warning::mysqli_warning(): Couldn't fetch mysqli in %s on line %d

Warning: mysqli_warning::mysqli_warning(): invalid object or resource mysqli_stmt
 in %s on line %d

Warning: mysqli_warning::mysqli_warning(): invalid object or resource mysqli_stmt
 in %s on line %d

Warning: mysqli_warning::mysqli_warning(): invalid class argument in /home/nixnutz/php6_mysqlnd/ext/mysqli/tests/mysqli_class_mysqli_warning.php on line 19

Warning: mysqli_warning::mysqli_warning(): No warnings found in %s on line %d
Parent class:
bool(false)

Methods:
ok

Class variables:

Object variables:

Magic, magic properties:
warning->message = ''
warning->sqlstate= ''
warning->errno = ''

Access to undefined properties:

warning->unknown = ''
done!