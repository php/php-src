<?php
// optional functions

include('config.inc');


$db = pg_connect($conn_str);
pg_query($db, 'LISTEN test_msg');
pg_query($db, 'NOTIFY test_msg');

$msg = pg_get_notify($db);

isset($msg['message'],$msg['pid']) ? print 'OK' : print 'NG';


?>