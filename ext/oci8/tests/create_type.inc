<?php

    if ($c) {
        $ora_sql = "DROP TYPE
                                ".$type_name."
                   ";

        $statement = oci_parse($c,$ora_sql);
        @oci_execute($statement);

        $ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF NUMBER(11)";

        $statement = oci_parse($c,$ora_sql);
        oci_execute($statement);
    }

?>
