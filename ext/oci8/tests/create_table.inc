<?php

    if ($c) {
        $ora_sql = "DROP TABLE
                                ".$schema.$table_name."
                   ";

        $statement = OCIParse($c, $ora_sql);
        @OCIExecute($statement);

        $ora_sql = "CREATE TABLE 
                                ".$schema.$table_name." (id NUMBER, value NUMBER, blob BLOB, clob CLOB, string VARCHAR(10)) 
                   ";
                      
        $statement = OCIParse($c,$ora_sql);
        OCIExecute($statement);
    }

?>
