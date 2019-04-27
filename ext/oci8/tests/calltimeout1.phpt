--TEST--
oci_set_call_timeout: test timing out 
--SKIPIF--
<?php
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) {
    die("skip needs to be run as a user with access to DBMS_LOCK");
}
preg_match('/^[[:digit:]]+/', oci_client_version(), $matches);
if (!(isset($matches[0]) && $matches[0] >= 18)) {
    die("skip works only with Oracle 18c or greater version of Oracle client libraries");
}

?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

function mysleep($c, $t)
{
    $s = @oci_parse($c, "begin dbms_lock.sleep(:t); end;");
    if (!$s) {
        $m = oci_error($c);
        echo "Execute error was ", $m['message'], "\n";
        return;
    }
    @oci_bind_by_name($s, ":t", $t);
    $r = @oci_execute($s);
    if ($r) {
        echo "Execute succeeded\n";
    } else {
        $m = oci_error($s);
        echo "Execute error was ", $m['message'], "\n";
    }
}

echo "Test 1\n";
oci_set_call_timeout($c, 4000);  // milliseconds
$r = mysleep($c, 8);             // seconds

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
Execute error was ORA-03136: %s
===DONE===
