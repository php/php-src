--TEST--
PDO Common: Bug #44159 (Crash: $pdo->setAttribute(PDO::STATEMENT_ATTR_CLASS, NULL))
--SKIPIF--
<?php
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
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

$attrs = array(PDO::ATTR_STATEMENT_CLASS, PDO::ATTR_STRINGIFY_FETCHES, PDO::NULL_TO_STRING);

foreach ($attrs as $attr) {
    try {
        var_dump($pdo->setAttribute($attr, NULL));
    } catch (\Error $e) {
        echo  get_class($e), ': ', $e->getMessage(), \PHP_EOL;
    }
    try {
        var_dump($pdo->setAttribute($attr, 1));
    } catch (\Error $e) {
        echo  get_class($e), ': ', $e->getMessage(), \PHP_EOL;
    }
    try {
        var_dump($pdo->setAttribute($attr, 'nonsense'));
    } catch (\Error $e) {
        echo  get_class($e), ': ', $e->getMessage(), \PHP_EOL;
    }
}

@unlink(__DIR__."/foo.db");

?>
--EXPECTF--
Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS requires format array(classname, array(ctor_args)); the classname must be a string specifying an existing class in %s on line %d

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error in %s on line %d
bool(false)

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS requires format array(classname, array(ctor_args)); the classname must be a string specifying an existing class in %s on line %d

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error in %s on line %d
bool(false)

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS requires format array(classname, array(ctor_args)); the classname must be a string specifying an existing class in %s on line %d

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error in %s on line %d
bool(false)
TypeError: Attribute value must be int for selected attribute, null given
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
