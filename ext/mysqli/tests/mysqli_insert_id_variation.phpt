--TEST--
Checking last_insert_id after different operations
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
                        $host, $user, $db, $port, $socket);

$link->query("DROP TABLE IF EXISTS test_insert_id_var");
$link->query("CREATE TABLE test_insert_id_var (id INT auto_increment, PRIMARY KEY (id))");
$link->query("INSERT INTO test_insert_id_var VALUES (null)");
$i = $link->insert_id;

if (!$i) {
    printf("[001] Got no valid insert id: %s", var_export($i, true));
    die();
}


$link->options(MYSQLI_OPT_LOCAL_INFILE, false);
if ($i != $link->insert_id || $i != mysqli_insert_id($link)) {
    printf("[002] mysqli_option changes insert_id: %s", var_export($link->insert_id, true));
    die();
}

/*
$link->dump_debug_info();
if ($i != $link->insert_id || $i != mysqli_insert_id($link)) {
    printf("[003] mysqli_debug_info changes insert_id: %s", var_export($link->insert_id, true));
    die();
}
*/

$link->stat();
if ($i != $link->insert_id || $i != mysqli_insert_id($link)) {
    printf("[004] mysqli_stat changes insert_id: %s", var_export($link->insert_id, true));
    die();
}

/*$link->kill($link->thread_id);
if ($i != $link->insert_id || $i != mysqli_insert_id($link)) {
    printf("[005] mysqli_kill changes insert_id: %s", var_export($link->insert_id, true));
    die();
}*/

$link->ping();
if ($i != $link->insert_id || $i != mysqli_insert_id($link)) {
    printf("[006] mysqli_ping changes insert_id: %s", var_export($link->insert_id, true));
    die();
}

/*
mysqlnd resets the IDE to 0
libmysql doesn't

$link->change_user ($user, $passwd, $db);
if (0 != $link->insert_id || 0 != mysqli_insert_id($link)) {
    printf("[007] mysqli_change_user changes insert_id: %s", var_export($link->insert_id, true));
    die();
}
*/

$stmt = $link->prepare("SELECT 1");
if ($i != $link->insert_id || $i != mysqli_insert_id($link)) {
    printf("[008a] mysqli_prepare changes insert_id: %s", var_export($link->insert_id, true));
    die();
}
echo mysqli_error($link);
if (0 != $stmt->insert_id || 0 != mysqli_stmt_insert_id($stmt)) {
    printf("[008b] mysqli_stmt doesn't initialise insert_id: %s", var_export($stmt->insert_id, true));
    die();
}

unset($stmt);
if ($i != $link->insert_id || $i != mysqli_insert_id($link)) {
    printf("[009] stmt free changes insert_id: %s", var_export($link->insert_id, true));
    die();
}

$link->query("DROP TABLE IF EXISTS test_insert_id_var");

echo "DONE";
?>
--CLEAN--
<?php
require_once 'connect.inc';
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_insert_id_var"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
DONE
