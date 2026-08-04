--TEST--
pg_fetch_object() constructor handling: ctor_params validation, throwing constructor, property visibility
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include 'inc/config.inc';

class NoCtor {}

class ThrowingCtor {
    public function __construct() {
        throw new RuntimeException('boom');
    }
    public function __destruct() {
        echo "ThrowingCtor::__destruct called (BUG)\n";
    }
}

class SeesProps {
    public function __construct() {
        echo "ctor sees: num={$this->num}, str={$this->str}\n";
    }
    public function __destruct() {
        echo "SeesProps::__destruct called\n";
    }
}

$table_name = "pg_fetch_object_ctor_paths";
$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int, str text)");
pg_query($db, "INSERT INTO {$table_name} VALUES(1, 'hello')");

$sql = "SELECT * FROM {$table_name} WHERE num = 1";

// 1) ctor_params on a class with no constructor must throw ValueError
try {
    pg_fetch_object(pg_query($db, $sql), null, 'NoCtor', [1, 2]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

// 2) Constructor that throws: __destruct must NOT run on the partially constructed object
try {
    pg_fetch_object(pg_query($db, $sql), null, 'ThrowingCtor');
} catch (RuntimeException $e) {
    echo "caught: ", $e->getMessage(), "\n";
}

// 3) Constructor sees row properties already merged onto $this
$obj = pg_fetch_object(pg_query($db, $sql), null, 'SeesProps');
unset($obj);

echo "Ok\n";
?>
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS pg_fetch_object_ctor_paths");
?>
--EXPECT--
pg_fetch_object(): Argument #4 ($constructor_args) must be empty when the specified class (NoCtor) does not have a constructor
caught: boom
ctor sees: num=1, str=hello
SeesProps::__destruct called
Ok
