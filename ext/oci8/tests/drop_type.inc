<?php

    if ($c) {
        $ora_sql = "DROP TYPE
                                ".$type_name."
                   ";
                      
        $statement = OCIParse($c,$ora_sql);
        OCIExecute($statement);
    }

?>
