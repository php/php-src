<?php
if ($c) {
    $ora_sql = "DROP TABLE ".$schema.$table_name;                      
    $statement = oci_parse($c,$ora_sql);
    oci_execute($statement);
}
?>
