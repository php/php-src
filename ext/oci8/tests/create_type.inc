<?php

    if ($c) {
        $ora_sql = "DROP TYPE
                                ".$type_name."
                   ";

        $statement = OCIParse($c,$ora_sql);
        @OCIExecute($statement);

        $ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF NUMBER(11)";
                      
        $statement = OCIParse($c,$ora_sql);
        OCIExecute($statement);
    }

?>
