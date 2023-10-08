--TEST--
mysqli_get_warnings() - TODO
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
if (!$TEST_EXPERIMENTAL)
    die("skip - experimental (= unsupported) feature");
?>
--FILE--
<?php
    require_once 'connect.inc';

    $tmp    = NULL;
    $link   = NULL;

    if (!is_null($tmp = @mysqli_get_warnings()))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @mysqli_get_warnings($link)))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @mysqli_get_warnings('')))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
        printf("[003] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
    }

    if (false !== ($tmp = mysqli_get_warnings($link))) {
        printf("[004] Expecting boolean/false, got %s/%s\n", gettype($tmp), (is_object($tmp) ? var_dump($tmp, true) : $tmp));
    }

    if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
        printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "CREATE TABLE test (id SMALLINT)"))
        printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, "INSERT INTO test (id) VALUES (1000000)"))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!is_object($warning = mysqli_get_warnings($link)) || 'mysqli_warning' != get_class($warning)) {
        printf("[008] Expecting object/mysqli_warning, got %s/%s\n", gettype($tmp), (is_object($tmp) ? var_dump($tmp, true) : $tmp));
    }

    if (!method_exists($warning, 'next'))
        printf("[009] Borked object, method next is missing\n");

    $properties = array_merge(get_object_vars($warning), get_class_vars(get_class($warning)));
    if (!empty($properties))
        printf("[010] Properties have always been magic, hidden things - why are they visible now, a BC break...\n");

    if ((!is_string($warning->message)) || ('' == $warning->message)) /* NULL or not there at all */
        printf("[011] Expecting string/not empty, got %s/%s\n", gettype($warning->message), $warning->message);

    if ((!is_string($warning->sqlstate)) || ('' == $warning->sqlstate)) /* NULL or not there at all */
        printf("[012] Expecting string/not empty, got %s/%s\n", gettype($warning->sqlstate), $warning->sqlstate);

    if ((!is_int($warning->errno)) || (0 == $warning->errno)) /* NULL or not there at all */
        printf("[013] Expecting int/not 0, got %s/%s\n", gettype($warning->errno), $warning->errno);

    if (false !== ($tmp = $warning->next()))
        printf("[014] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_query($link, "INSERT INTO test (id) VALUES (1000000), (1000001)"))
        printf("[015] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (($tmp = mysqli_warning_count($link)) !== 2)
        printf("[016] Expecting 2 warnings, got %d warnings", $tmp);

    if (!is_object($warning = mysqli_get_warnings($link)) || 'mysqli_warning' != get_class($warning)) {
        printf("[017] Expecting object/mysqli_warning, got %s/%s\n", gettype($tmp), (is_object($tmp) ? var_dump($tmp, true) : $tmp));
    }

    if (true !== ($tmp = $warning->next()))
        printf("[018] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    if (false !== ($tmp = $warning->next()))
        printf("[020] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_close($link);


    $mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    if (!$mysqli->query("DROP TABLE IF EXISTS t1"))
        printf("[022] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$mysqli->query("CREATE TABLE t1 (a smallint)"))
        printf("[023] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $warning = new mysqli_warning($mysqli);

    if (!is_string($warning->message) || ('' == $warning->message))
        printf("[025] Expecting string, got %s/%s", gettype($warning->message), $warning->message);

    if (!$mysqli->query("DROP TABLE t1"))
        printf("[026] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    /* Yes, I really want to check if the object property is empty */
    $mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    $warning = new mysqli_warning($mysqli);
    if (false !== ($tmp = $warning->next()))
        printf("[028] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if ('' != ($tmp = $warning->message))
        printf("[029] Expecting string/empty, got %s/%s\n", gettype($tmp), $tmp);

    $mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

    if (!$mysqli->query("DROP TABLE IF EXISTS t1"))
        printf("[031] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!$mysqli->query("CREATE TABLE t1 (a smallint)"))
        printf("[032] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    /* out of range, three warnings */
    if (!$mysqli->query("INSERT IGNORE INTO t1(a) VALUES (65536), (65536), (65536)"))
        printf("[033] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $warning = new mysqli_warning($mysqli);
        $i = 1;
    while ($warning->next() && ('' != ($tmp = $warning->message))) {
        $i++;
    }
    if (3 != $i)
        printf("[034] Expecting three warnings, got %d warnings\n", $i);

    $stmt = mysqli_stmt_init();
    $warning = new mysqli_warning($stmt);
    if (false !== ($tmp = $warning->next()))
        printf("[035] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    print "done!";
?>
<?php
require_once 'connect.inc';
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

if (!mysqli_query($link, "DROP TABLE IF EXISTS t1"))
    printf("[c003] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
done!
