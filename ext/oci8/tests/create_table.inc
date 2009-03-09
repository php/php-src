<?php
    if ($c) {
        $ora_sql = "DROP TABLE ".$schema.$table_name;
        $statement = oci_parse($c, $ora_sql);
        @oci_execute($statement);

        $ora_sql = "CREATE TABLE ".$schema.$table_name." (id NUMBER, value NUMBER, blob BLOB, clob CLOB, string VARCHAR(10))";
        $statement = oci_parse($c, $ora_sql);
        oci_execute($statement);
    }
?>
