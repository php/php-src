--TEST--
mysqli_options()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $valid_options = array(
        MYSQLI_READ_DEFAULT_GROUP 			=> "MYSQLI_READ_DEFAULT_GROUP",
        MYSQLI_READ_DEFAULT_FILE 			=> "MYSQLI_READ_DEFAULT_FILE",
        MYSQLI_OPT_CONNECT_TIMEOUT			=> "MYSQLI_OPT_CONNECT_TIMEOUT",
        MYSQLI_OPT_LOCAL_INFILE				=> "MYSQLI_OPT_LOCAL_INFILE",
        MYSQLI_INIT_COMMAND					=> "MYSQLI_INIT_COMMAND",
        MYSQLI_SET_CHARSET_NAME				=> "MYSQLI_SET_CHARSET_NAME",
        MYSQLI_OPT_SSL_VERIFY_SERVER_CERT 	=> "MYSQLI_OPT_SSL_VERIFY_SERVER_CERT",
    );

    if ($IS_MYSQLND && defined('MYSQLI_OPT_NET_CMD_BUFFER_SIZE'))
        $valid_options[] = constant('MYSQLI_OPT_NET_CMD_BUFFER_SIZE');
    if ($IS_MYSQLND && defined('MYSQLI_OPT_NET_READ_BUFFER_SIZE'))
        $valid_options[] = constant('MYSQLI_OPT_NET_READ_BUFFER_SIZE');
    if ($IS_MYSQLND && defined('MYSQLI_OPT_INT_AND_FLOAT_NATIVE'))
        $valid_options[] = constant('MYSQLI_OPT_INT_AND_FLOAT_NATIVE');

    $tmp    = NULL;
    $link   = NULL;

    if (!is_null($tmp = @mysqli_options()))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @mysqli_options($link)))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    $link = mysqli_init();

    /* set it twice, checking if memory for the previous one is correctly freed */
    mysqli_options($link, MYSQLI_SET_CHARSET_NAME, "utf8");
    mysqli_options($link, MYSQLI_SET_CHARSET_NAME, "latin1");

    if (!is_null($tmp = @mysqli_options($link, MYSQLI_OPT_CONNECT_TIMEOUT)))
        printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @mysqli_options($link, "s", 'extra_my.cnf')))
        printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @mysqli_options($link, MYSQLI_INIT_COMMAND, 'SET AUTOCOMMIT=0', 'foo')))
        printf("[005] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    // print "run_tests.php don't fool me with your 'ungreedy' expression '.+?'!\n";
    var_dump("MYSQLI_READ_DEFAULT_GROUP",	mysqli_options($link, MYSQLI_READ_DEFAULT_GROUP, 'extra_my.cnf'));
    var_dump("MYSQLI_READ_DEFAULT_FILE",	mysqli_options($link, MYSQLI_READ_DEFAULT_FILE, 'extra_my.cnf'));
    var_dump("MYSQLI_OPT_CONNECT_TIMEOUT",	mysqli_options($link, MYSQLI_OPT_CONNECT_TIMEOUT, 10));
    var_dump("MYSQLI_OPT_LOCAL_INFILE",		mysqli_options($link, MYSQLI_OPT_LOCAL_INFILE, 1));
    var_dump("MYSQLI_INIT_COMMAND",			mysqli_options($link, MYSQLI_INIT_COMMAND, array('SET AUTOCOMMIT=0', 'SET AUTOCOMMIT=1')));

    if (!$link2 = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[006] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (!$res = mysqli_query($link2, 'SELECT version() AS server_version'))
        printf("[007] [%d] %s\n", mysqli_errno($link2), mysqli_error($link2));
    $tmp = mysqli_fetch_assoc($res);
    mysqli_free_result($res);
    $version = explode('.', $tmp['server_version']);
    if (empty($version))
        printf("[008] Cannot determine server version, need MySQL Server 4.1+ for the test!\n");

    if ($version[0] <= 4 && $version[1] < 1)
        printf("[009] Need MySQL Server 4.1+ for the test!\n");

    if (!$res = mysqli_query($link2, "SHOW CHARACTER SET"))
        printf("[010] Cannot get list of available character sets, [%d] %s\n",
            mysqli_errno($link2), mysqli_error($link2));

    $charsets = array();
    while ($row = mysqli_fetch_assoc($res))
        $charsets[] = $row;
    mysqli_free_result($res);
    mysqli_close($link2);

    foreach ($charsets as $charset) {
        $k = $charset['Charset'];
        /* The server currently 17.07.2007 can't handle data sent in ucs2 */
        /* The server currently 16.08.2010 can't handle data sent in utf16 and utf32 */
        if ($charset['Charset'] == 'ucs2' || $charset['Charset'] == 'utf16' || $charset['Charset'] == 'utf32') {
            continue;
        }
        if (true !== mysqli_options($link, MYSQLI_SET_CHARSET_NAME, $charset['Charset'])) {
            printf("[009] Setting charset name '%s' has failed\n", $charset['Charset']);
        }
    }

    var_dump("MYSQLI_READ_DEFAULT_GROUP",	mysqli_options($link, MYSQLI_READ_DEFAULT_GROUP, 'extra_my.cnf'));
    var_dump("MYSQLI_READ_DEFAULT_FILE",	mysqli_options($link, MYSQLI_READ_DEFAULT_FILE, 'extra_my.cnf'));
    var_dump("MYSQLI_OPT_CONNECT_TIMEOUT",	mysqli_options($link, MYSQLI_OPT_CONNECT_TIMEOUT, 10));
    var_dump("MYSQLI_OPT_LOCAL_INFILE",		mysqli_options($link, MYSQLI_OPT_LOCAL_INFILE, 1));
    var_dump("MYSQLI_INIT_COMMAND",			mysqli_options($link, MYSQLI_INIT_COMMAND, 'SET AUTOCOMMIT=0'));

    /* mysqli_real_connect() */
    var_dump("MYSQLI_CLIENT_SSL",			mysqli_options($link, MYSQLI_CLIENT_SSL, 'not a mysqli_option'));

    mysqli_close($link);

    echo "Link closed";
    var_dump("MYSQLI_INIT_COMMAND", mysqli_options($link, MYSQLI_INIT_COMMAND, 'SET AUTOCOMMIT=1'));
    print "done!";
?>
--EXPECTF--
%s(25) "MYSQLI_READ_DEFAULT_GROUP"
bool(true)
%s(24) "MYSQLI_READ_DEFAULT_FILE"
bool(true)
%s(26) "MYSQLI_OPT_CONNECT_TIMEOUT"
bool(true)
%s(23) "MYSQLI_OPT_LOCAL_INFILE"
bool(true)

Notice: Array to string conversion in %s on line %d
%s(19) "MYSQLI_INIT_COMMAND"
bool(true)
%s(25) "MYSQLI_READ_DEFAULT_GROUP"
bool(true)
%s(24) "MYSQLI_READ_DEFAULT_FILE"
bool(true)
%s(26) "MYSQLI_OPT_CONNECT_TIMEOUT"
bool(true)
%s(23) "MYSQLI_OPT_LOCAL_INFILE"
bool(true)
%s(19) "MYSQLI_INIT_COMMAND"
bool(true)
%s(17) "MYSQLI_CLIENT_SSL"
bool(false)
Link closed
Warning: mysqli_options(): Couldn't fetch mysqli in %s line %d
%s(19) "MYSQLI_INIT_COMMAND"
bool(false)
done!
