--TEST--
Test last_query_error property on failed query
--FILE--
<?php
try {
    $mysqli = new mysqli("localhost", "user", "pass", "database");
    $mysqli->query("SELECT * FROM unknown_table");
} catch(mysqli_sql_exception $e) {
    echo $mysqli->last_query_error;
}
--EXPECT--
Test passed
?>
