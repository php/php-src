--TEST--
new mysqli()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require_once 'connect.inc';

    $tmp    = NULL;
    $link   = NULL;

    $obj = new stdClass();

    if ($mysqli = new mysqli($host, $user . 'unknown_really', $passwd . 'non_empty', $db, $port, $socket) && !mysqli_connect_errno())
        printf("[003] Can connect to the server using host=%s, user=%s, passwd=***non_empty, dbname=%s, port=%s, socket=%s\n",
            $host, $user . 'unknown_really', $db, $port, $socket);

    if (false !== $mysqli)
        printf("[004] Expecting boolean/false, got %s/%s\n", gettype($mysqli), $mysqli);

    // Run the following tests without an anoynmous MySQL user and use a password for the test user!
    ini_set('mysqli.default_socket', $socket);
    $mysqli = new mysqli($host, $user, $passwd, $db, $port);
    if (0 !== mysqli_connect_errno()) {
        printf("[005] Usage of mysqli.default_socket failed\n") ;
    } else {
        $mysqli->close();
    }

    ini_set('mysqli.default_port', $port);
    $mysqli = new mysqli($host, $user, $passwd, $db);
    if (0 !== mysqli_connect_errno()) {
        printf("[006] Usage of mysqli.default_port failed\n") ;
    } else {
        $mysqli->close();
    }

    ini_set('mysqli.default_pw', $passwd);
    $mysqli = new mysqli($host, $user);
    if (0 !== mysqli_connect_errno()) {
        printf("[007] Usage of mysqli.default_pw failed\n") ;
    } else {
        $mysqli->close();
    }

    ini_set('mysqli.default_user', $user);
    $mysqli = new mysqli($host);
    if (0 !== mysqli_connect_errno()) {
        printf("[008] Usage of mysqli.default_user failed\n") ;
    } else {
        $mysqli->close();
    }

    ini_set('mysqli.default_host', $host);
    $mysqli = new mysqli();
    if (0 !== mysqli_connect_errno()) {
        printf("[012] Failed to create mysqli object\n");
    } else {
        // There shall be NO connection! Using new mysqli(void) shall not use defaults for a connection!
        // We had long discussions on this and found that the ext/mysqli API as
        // such is broken. As we can't fix it, we document how it has behaved from
        // the first day on. And that's: no connection.
        try {
            $mysqli->query('SELECT 1');
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }
    }

    ini_set('mysqli.default_host', 'p:' . $host);
    $mysqli = new mysqli();
    // There shall be NO connection! Using new mysqli(void) shall not use defaults for a connection!
    // We had long discussions on this and found that the ext/mysqli API as
    // such is broken. As we can't fix it, we document how it has behaved from
    // the first day on. And that's: no connection.
    try {
        $mysqli->query('SELECT 1');
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "... and now Exceptions\n";
    mysqli_report(MYSQLI_REPORT_OFF);
    mysqli_report(MYSQLI_REPORT_STRICT);

    try {
        $mysqli = new mysqli($host, $user . 'unknown_really', $passwd . 'non_empty', $db, $port, $socket);
        printf("[016] Can connect to the server using host=%s, user=%s, passwd=***non_empty, dbname=%s, port=%s, socket=%s\n",
            $host, $user . 'unknown_really', $db, $port, $socket);
        $mysqli->close();
    } catch (mysqli_sql_exception $e) {
        printf("%s\n", $e->getMessage());
    }

    ini_set('mysqli.default_socket', $socket);
    try {
        $mysqli = new mysqli($host, $user, $passwd, $db, $port);
        $mysqli->close();
    } catch (mysqli_sql_exception $e) {
        printf("%s\n", $e->getMessage());
        printf("[017] Usage of mysqli.default_socket failed\n") ;
    }

    ini_set('mysqli.default_port', $port);
    try {
        $mysqli = new mysqli($host, $user, $passwd, $db);
        $mysqli->close();
    } catch (mysqli_sql_exception $e) {
        printf("%s\n", $e->getMessage());
        printf("[018] Usage of mysqli.default_port failed\n") ;
    }

    ini_set('mysqli.default_pw', $passwd);
    try {
        $mysqli = new mysqli($host, $user);
        $mysqli->close();
    } catch (mysqli_sql_exception $e) {
        printf("%s\n", $e->getMessage());
        printf("[019] Usage of mysqli.default_pw failed\n");
    }

    ini_set('mysqli.default_user', $user);
    try {
        $mysqli = new mysqli($host);
        $mysqli->close();
    } catch (mysqli_sql_exception $e) {
        printf("%s\n", $e->getMessage());
        printf("[020] Usage of mysqli.default_user failed\n") ;
    }

    ini_set('mysqli.default_host', $host);
    try {
        /* NOTE that at this point one must use a different syntax! */
        $mysqli = mysqli_init();
        $mysqli->real_connect();
        assert(0 === mysqli_connect_errno());
        $mysqli->close();
        assert(0 === mysqli_connect_errno());
    } catch (mysqli_sql_exception $e) {
        printf("%s\n", $e->getMessage());
        printf("[021] Usage of mysqli.default_host failed\n");
    }

    print "done!";
?>
--EXPECTF--
Warning: mysqli::__construct(): (%s/%d): Access denied for user '%sunknown%s'@'%s' %r(\(using password: \w+\) ){0,1}%rin %s on line %d
mysqli object is not fully initialized
mysqli object is not fully initialized
... and now Exceptions
Access denied for user '%s'@'%s'%r( \(using password: \w+\)){0,1}%r
done!
