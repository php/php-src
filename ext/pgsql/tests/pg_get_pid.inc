<?php
// optional functions

include('config.inc');


$db = pg_connect($conn_str);
$pid = pg_get_pid($db);

is_integer($pid) ? print 'OK' : print 'NG';


?>