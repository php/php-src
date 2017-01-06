--TEST--
PDO Common: Bug #44159 (Crash: $pdo->setAttribute(PDO::STATEMENT_ATTR_CLASS, NULL))
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo')) die('skip PDO not available');
try {
	$pdo = new PDO("sqlite:".__DIR__."/foo.db");
} catch (Exception $e) {
	die("skip PDP_SQLITE not available");
}
?>
--FILE--
<?php
$pdo = new PDO("sqlite:".__DIR__."/foo.db");

$attrs = array(PDO::ATTR_STATEMENT_CLASS, PDO::ATTR_STRINGIFY_FETCHES, PDO::NULL_TO_STRING);

foreach ($attrs as $attr) {
	var_dump($pdo->setAttribute($attr, NULL));
	var_dump($pdo->setAttribute($attr, 1));
	var_dump($pdo->setAttribute($attr, 'nonsense'));
}

@unlink(__DIR__."/foo.db");

?>
--EXPECTF--
Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS requires format array(classname, array(ctor_args)); the classname must be a string specifying an existing class in %s on line %d
bool(false)

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS requires format array(classname, array(ctor_args)); the classname must be a string specifying an existing class in %s on line %d
bool(false)

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS requires format array(classname, array(ctor_args)); the classname must be a string specifying an existing class in %s on line %d
bool(false)

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: attribute value must be an integer in %s on line %d
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
