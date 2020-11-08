--TEST--
PDO_OCI: Bug #69356 (PDOStatement::debugDumpParams() truncates query)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php
require(__DIR__ . '/../../pdo/tests/pdo_test.inc');

$db = PDOTest::factory();
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$stmt = $db->query("
    SELECT '
        Dumps the information contained by a prepared statement directly on the output. It will provide the SQL query in use, the number of parameters used (Params), the list of parameters, with their name, type (paramtype) as an integer, their key name or position, and the position in the query (if this is supported by the PDO driver, otherwise, it will be -1).
        This is a debug function, which dump directly the data on the normal output.
        Tip:
        As with anything that outputs its result directly to the browser, the output-control functions can be used to capture the output of this function, and save it in a string (for example).
        This will only dumps the parameters in the statement at the moment of the dump. Extra parameters are not stored in the statement, and not displayed.
    ' FROM DUAL
");
var_dump($stmt->debugDumpParams());
?>
--EXPECT--
SQL: [844] 
    SELECT '
        Dumps the information contained by a prepared statement directly on the output. It will provide the SQL query in use, the number of parameters used (Params), the list of parameters, with their name, type (paramtype) as an integer, their key name or position, and the position in the query (if this is supported by the PDO driver, otherwise, it will be -1).
        This is a debug function, which dump directly the data on the normal output.
        Tip:
        As with anything that outputs its result directly to the browser, the output-control functions can be used to capture the output of this function, and save it in a string (for example).
        This will only dumps the parameters in the statement at the moment of the dump. Extra parameters are not stored in the statement, and not displayed.
    ' FROM DUAL

Params:  0
NULL
