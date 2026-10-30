--TEST--
mysqli_driver class
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

$driver = new mysqli_driver();

$client_info = mysqli_get_client_info();
if (($tmp = $driver->client_info) !== $client_info) {
    printf("[002] Expecting %s/%s, got %s/%s\n",
        gettype($client_info), $client_info,
        gettype($tmp), $tmp);
}

$client_version = mysqli_get_client_version();
if (($tmp = $driver->client_version) !== $client_version) {
    printf("[003] Expecting %s/%s, got %s/%s\n",
    gettype($client_version), $client_version,
    gettype($tmp), $tmp);
}

$all_modes = [
    MYSQLI_REPORT_ALL,
    MYSQLI_REPORT_STRICT,
    MYSQLI_REPORT_STRICT|MYSQLI_REPORT_ERROR,
    MYSQLI_REPORT_STRICT|MYSQLI_REPORT_INDEX,
    MYSQLI_REPORT_ERROR,
    MYSQLI_REPORT_ERROR|MYSQLI_REPORT_INDEX,
    MYSQLI_REPORT_INDEX,
    MYSQLI_REPORT_OFF
];
$report_mode = $driver->report_mode;
if (!is_int($report_mode)) {
    printf("[005] Expecting int/any, got %s/%s\n",
    gettype($report_mode), $report_mode);
}

if (!in_array($report_mode, $all_modes)) {
    printf("[006] Illegal report mode returned? Got %s, expected %s\n",
        $report_mode, implode(', ', $all_modes));
}

$driver->report_mode = MYSQLI_REPORT_STRICT;
$ok = false;
try {
    if ($link = my_mysqli_connect($host, $user . 'unknown_really', $passwd . 'non_empty', $db, $port, $socket)) {
        printf("[007] Can connect to the server using host=%s, user=%s, passwd=***non_empty, dbname=%s, port=%s, socket=%s\n",
        $host, $user . 'unknown_really', $db, $port, $socket);
    }
    mysqli_close($link);
} catch (mysqli_sql_exception $e) {
    $ok = true;
    if ('' == $e->getMessage()) {
        printf("[008] getMessage() has returned an empty string.\n");
    }
    if ('' == $e->getCode()) {
        printf("[009] getCode() has returned an empty string.\n");
    }
    if ('' == $e->getFile()) {
        printf("[010] getFile() has returned an empty string.\n");
    }
    if ('' == $e->getLine()) {
        printf("[011] getLine() has returned an empty string.\n");
    }
    if ([] == $e->getTrace()) {
        printf("[012] getTrace() has returned an empty array.\n");
    }
    if ('' == $e->getTraceAsString()) {
        printf("[013] getTraceAsString() has returned an empty string.\n");
    }
    if ('' == $e->__toString()) {
        printf("[014] __toString() has returned an empty string.\n");
    }
}
if (!$ok) {
    printf("[015] Error reporting mode has not been switched to exceptions and or no exception thrown\n");
}

$driver->report_mode = MYSQLI_REPORT_OFF;
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    printf("[016] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
}
mysqli_query($link, "NO_SQL");
mysqli_close($link);

$driver->report_mode = MYSQLI_REPORT_ERROR;

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    printf("[017] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
}
mysqli_query($link, "NO_SQL");
mysqli_close($link);

if (MYSQLI_REPORT_ERROR !== $driver->report_mode) {
    printf("[018] Error reporting mode should be different\n");
}

/* TODO - more report testing should go in here, but it's not really documented what behaviour is expected */

$driver->report_mode = $report_mode;

print "done!";
?>
--EXPECTF--
Warning: mysqli_query(): (%d/%d): You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near 'NO_SQL' at line 1 in %s on line %d
done!
