--TEST--
Fetching results from tables of different charsets.
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');

if (!function_exists('mysqli_set_charset')) {
    die('skip mysqli_set_charset() not available');
}
?>
--FILE--
<?php
    require_once("connect.inc");

    $tmp	= NULL;
    $link	= NULL;
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (!$res = mysqli_query($link, 'SELECT version() AS server_version'))
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);
    $version = explode('.', $tmp['server_version']);
    if (empty($version))
        printf("[003] Cannot determine server version, need MySQL Server 4.1+ for the test!\n");

    if ($version[0] <= 4 && $version[1] < 1)
        printf("[004] Need MySQL Server 4.1+ for the test!\n");

    if (!$res = mysqli_query($link, "SHOW CHARACTER SET"))
        printf("[005] Cannot get list of available character sets, [%d] %s\n",
            mysqli_errno($link), mysqli_error($link));

    $charsets = array();
    while ($row = mysqli_fetch_assoc($res))
        $charsets[] = $row;
    mysqli_free_result($res);

    foreach ($charsets as $charset) {
        $k = $charset['Charset'];
        /* The server currently 17.07.2007 can't handle data sent in ucs2 */
        /* The server currently 16.08.2010 can't handle data sent in utf16 and utf32 */
        /* The server currently 02.09.2011 can't handle data sent in utf16le */
        if ($charset['Charset'] == 'ucs2' || $charset['Charset'] == 'utf16' || $charset['Charset'] == 'utf32' || 'utf16le' == $charset['Charset']) {
            continue;
        }

        if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
            printf("[006 + %s] [%d] %s\n", $k, mysqli_errno($link), mysqli_error($link));

        $sql = sprintf("CREATE TABLE test(id INT, label CHAR(1)) CHARACTER SET '%s' ", $charset['Charset']);
        if (!mysqli_query($link, $sql)) {
            printf("[007 + %s] %s [%d] %s\n", $k, $sql, mysqli_errno($link), mysqli_error($link));
            continue;
        }

        if (!mysqli_set_charset($link, $charset['Charset'])) {
            printf("[008 + %s] [%d] %s\n", $k, mysqli_errno($link), mysqli_error($link));
            continue;
        }

        for ($i = 1; $i <= 3; $i++) {
            if (!mysqli_query($link, sprintf("INSERT INTO test (id, label) VALUES (%d, '%s')",
                                $i, mysqli_real_escape_string($link, chr(ord("a") + $i)))))
            {
                var_dump($charset['Charset']);
                printf("[009 + %s] [%d] %s\n", $k, mysqli_errno($link), mysqli_error($link));
                continue;
            }
        }

        if (!$res = mysqli_query($link, "SELECT id, label FROM test"))
            printf("[010 + %s] [%d] %s\n", $k, mysqli_errno($link), mysqli_error($link));

        for ($i = 1; $i <= 3; $i++) {

            if (!$tmp = mysqli_fetch_assoc($res))
                printf("[011 + %s] [%d] %s\n", $k, mysqli_errno($link), mysqli_error($link));

            if ($tmp['id'] != $i)
                printf("[012 + %s] Expecting %d, got %s, [%d] %s\n", $k,
                        $i, $tmp['id'],
                        mysqli_errno($link), mysqli_error($link));

            if ($tmp['label'] != chr(ord("a") + $i))
                printf("[013 + %s] Expecting %d, got %s, [%d] %s\n", $k,
                    chr(ord("a") + $i), $tmp['label'],
                    mysqli_errno($link), mysqli_error($link));

        }
        mysqli_free_result($res);
    }

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
done!
