--TEST--
mysqli_set_charset()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';
if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

if (!($res = mysqli_query($link, 'SELECT version() AS server_version')) ||
        !($tmp = mysqli_fetch_assoc($res))) {
    mysqli_close($link);
    die(sprintf("skip Cannot check server version, [%d] %s\n",
    mysqli_errno($link), mysqli_error($link)));
}
mysqli_free_result($res);
$version = explode('.', $tmp['server_version']);
if (empty($version)) {
    mysqli_close($link);
    die(sprintf("skip Cannot check server version, based on '%s'",
        $tmp['server_version']));
}

if ($version[0] <= 4 && $version[1] < 1) {
    mysqli_close($link);
    die(sprintf("skip Requires MySQL Server 4.1+\n"));
}

if ((($res = mysqli_query($link, 'SHOW CHARACTER SET LIKE "latin1"', MYSQLI_STORE_RESULT)) &&
        (mysqli_num_rows($res) == 1)) ||
        (($res = mysqli_query($link, 'SHOW CHARACTER SET LIKE "latin2"', MYSQLI_STORE_RESULT)) &&
        (mysqli_num_rows($res) == 1))
        ) {
    // ok, required latin1 or latin2 are available
    mysqli_close($link);
} else {
    die(sprintf("skip Requires character set latin1 or latin2\n"));
    mysqli_close($link);
}
?>
--FILE--
<?php
    require 'table.inc';

    if (!$res = mysqli_query($link, 'SELECT @@character_set_connection AS charset, @@collation_connection AS collation'))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);
    if (!$character_set_connection = $tmp['charset'])
        printf("[008] Cannot determine current character set and collation\n");

    $new_charset = ('latin1' == $character_set_connection) ? 'latin2' : 'latin1';
    if (!$res = mysqli_query($link, sprintf('SHOW CHARACTER SET LIKE "%s"', $new_charset), MYSQLI_STORE_RESULT))
        printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (mysqli_num_rows($res) == 0)
        printf("[010] Test will fail, because alternative test character set '%s' seems not supported\n", $new_charset);

    if (false !== ($ret = mysqli_set_charset($link, "this is not a valid character set")))
        printf("[011] Expecting boolean/false because of invalid character set, got %s/%s\n", gettype($ret), $ret);

    mysqli_close($link);
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[012] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (true !== ($ret = mysqli_set_charset($link, $new_charset)))
        printf("[013] Expecting boolean/true, got %s/%s\n", gettype($ret), $ret);

    if (!$res = mysqli_query($link, 'SELECT @@character_set_connection AS charset, @@collation_connection AS collation'))
        printf("[014] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);
    if ($new_charset !== $tmp['charset'])
        printf("[015] Character set not changed? Expecting %s, got %s\n", $new_charset, $tmp['charset']);

    if (!$res = mysqli_query($link, "SHOW CHARACTER SET"))
        printf("[016] Cannot get list of character sets\n");

    while ($tmp = mysqli_fetch_assoc($res)) {
        /* As of MySQL 8.0.28, `SHOW CHARACTER SET` contains utf8mb3, but that is not yet supported by mysqlnd */
        if ('ucs2' == $tmp['Charset'] || 'utf16' == $tmp['Charset'] || 'utf32' == $tmp['Charset'] || 'utf16le' == $tmp['Charset'] || 'utf8mb3' == $tmp['Charset'])
            continue;

        /* Uncomment to see where it hangs - var_dump($tmp); flush(); */
        if (!@mysqli_set_charset($link, $tmp['Charset'])) {
            printf("[017] Cannot set character set to '%s', [%d] %s\n", $tmp['Charset'],
                mysqli_errno($link), mysqli_error($link));
            continue;
        }

        /* Uncomment to see where it hangs - var_dump($tmp); flush(); */
        if (!mysqli_query($link, sprintf("SET NAMES %s", mysqli_real_escape_string($link, $tmp['Charset']))))
            printf("[018] Cannot run SET NAMES %s, [%d] %s\n", $tmp['Charset'], mysqli_errno($link), mysqli_error($link));
    }
    mysqli_free_result($res);

    // Make sure that set_charset throws an exception in exception mode
    mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
    try {
        $link->set_charset('invalid');
    } catch (\mysqli_sql_exception $exception) {
        echo "Exception: " . $exception->getMessage() . "\n";
    }

    try {
        $link->set_charset('ucs2');
    } catch (\mysqli_sql_exception $exception) {
        echo "Exception: " . $exception->getMessage() . "\n";
    }

    mysqli_close($link);

    try {
        mysqli_set_charset($link, $new_charset);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--CLEAN--
<?php
require_once 'clean_table.inc';
?>
--EXPECTF--
Exception: %s
Exception: Variable 'character_set_client' can't be set to the value of 'ucs2'
mysqli object is already closed
done!
