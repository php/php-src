<?php

    if ($connection) {
        $ora_sql = "CREATE TABLE 
                                ".$schema."php_test_table (id NUMBER, value NUMBER) 
                   ";
                      
        $statement = OCIParse($connection,$ora_sql);
        OCIExecute($statement);
    }

?>
