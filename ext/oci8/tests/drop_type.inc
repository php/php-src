<?php
if ($c) {
    $ora_sql = "DROP TYPE ".$type_name;
    $statement = oci_parse($c,$ora_sql);
    oci_execute($statement);
}
?>
