<?php
// drop test table 

include('config.inc');

pg_connect($conn_str);
pg_query($db, "DROP TABLE ".$table_name);

echo "OK";

?>