<?php
// optional functions

include('config.inc');

$db = pg_connect($conn_str);
var_dump(pg_ping($db));


?>