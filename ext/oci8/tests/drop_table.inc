<?php

    if ($connection) {
        $ora_sql = "DROP TABLE
                                ".$schema."php_test_table
                   ";
                      
        $statement = OCIParse($connection,$ora_sql);
        OCIExecute($statement);
    }

?>
