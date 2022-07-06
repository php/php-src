<?php
if ($c) {


    $stmtarray = array(
        "DROP TABLE ".$schema.$table_name,
        "CREATE TABLE ".$schema.$table_name." (id NUMBER, value NUMBER, blob BLOB, clob CLOB, string VARCHAR(10))"
    );

    oci8_test_sql_execute($c, $stmtarray);

}
?>
