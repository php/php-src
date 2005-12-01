<?php

    if ($c) {
        $ora_sql = "DROP TABLE
                                ".$schema.$table_name."
                   ";
                      
        $statement = OCIParse($c,$ora_sql);
        OCIExecute($statement);
    }

?>
