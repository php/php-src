--TEST--
mysqli_query()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    $tmp    = NULL;
    $link   = NULL;

    if (!is_null($tmp = @mysqli_query()))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @mysqli_query($link)))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    require('table.inc');

    if (false !== ($tmp = @mysqli_query($link, '')))
        printf("[002a] Expecting boolean/false got %s/%s\n", gettype($tmp), $tmp);

    if (NULL !== ($tmp = @mysqli_query($link, "SELECT 1 AS a", MYSQLI_USE_RESULT, "foo")))
        printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (false !== ($tmp = mysqli_query($link, 'THIS IS NOT SQL')))
        printf("[004] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (false !== ($tmp = mysqli_query($link, "SELECT 'this is sql but with backslash g'\g")))
        printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if ((0 === mysqli_errno($link)) || ('' == mysqli_error($link)))
        printf("[006] mysqli_errno()/mysqli_error should return some error\n");

    if (!$res = mysqli_query($link, "SELECT 'this is sql but with semicolon' AS valid ; "))
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    var_dump(mysqli_fetch_assoc($res));
    mysqli_free_result($res);

    if (!$res = mysqli_query($link, "SELECT 'a' AS ''"))
        printf("[007a] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    var_dump($tmp = mysqli_fetch_assoc($res));
    var_dump($tmp[""]);
    mysqli_free_result($res);

    if (false !== ($res = mysqli_query($link, 'SELECT "this is sql but with semicolon" AS valid ; SHOW VARIABLES')))
        printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (mysqli_get_server_version($link) > 50000) {
        // let's try to play with stored procedures
        mysqli_query($link, 'DROP PROCEDURE IF EXISTS p');
        if (mysqli_query($link, 'CREATE PROCEDURE p(OUT ver_param VARCHAR(25)) BEGIN SELECT VERSION() INTO ver_param; END;')) {
            $res = mysqli_query($link, 'CALL p(@version)');
            $res = mysqli_query($link, 'SELECT @version AS p_version');

            $tmp = mysqli_fetch_assoc($res);
            if (!is_array($tmp) || empty($tmp) || !isset($tmp['p_version']) || ('' == $tmp['p_version'])) {
                printf("[008a] Expecting array [%d] %s\n", mysqli_errno($link), mysqli_error($link));
                var_dump($tmp);
            }

            mysqli_free_result($res);
        } else {
            printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
        }

        mysqli_query($link, 'DROP FUNCTION IF EXISTS f');
        if (mysqli_query($link, 'CREATE FUNCTION f( ver_param VARCHAR(25)) RETURNS VARCHAR(25) DETERMINISTIC RETURN ver_param;')) {
            $res = mysqli_query($link, 'SELECT f(VERSION()) AS f_version');

            $tmp = mysqli_fetch_assoc($res);
            if (!is_array($tmp) || empty($tmp) || !isset($tmp['f_version']) || ('' == $tmp['f_version'])) {
                printf("[009a] Expecting array [%d] %s\n", mysqli_errno($link), mysqli_error($link));
                var_dump($tmp);
            }

            mysqli_free_result($res);
        } else {
            printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
        }
    }

    if (!is_object($res = mysqli_query($link, "SELECT id FROM test ORDER BY id", MYSQLI_USE_RESULT)))
        printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    mysqli_free_result($res);

    if (!is_object($res = mysqli_query($link, "SELECT id FROM test ORDER BY id", MYSQLI_STORE_RESULT)))
        printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    mysqli_free_result($res);

    if (false !== ($res = @mysqli_query($link, "SELECT id FROM test ORDER BY id", 1234)))
        printf("[013] Invalid mode should return false got %s/%s, [%d] %s\n",
            gettype($res), (is_object($res)) ? 'object' : $res,
            mysqli_errno($link), mysqli_error($link));


    mysqli_close($link);

    if (false !== ($tmp = mysqli_query($link, "SELECT id FROM test")))
        printf("[011] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

@mysqli_query($link, "DROP FUNCTION IF EXISTS f");
@mysqli_query($link, 'DROP PROCEDURE IF EXISTS p');

mysqli_close($link);
?>
--EXPECTF--
array(1) {
  ["valid"]=>
  string(30) "this is sql but with semicolon"
}
array(1) {
  [""]=>
  string(1) "a"
}
string(1) "a"

Warning: mysqli_query(): Couldn't fetch mysqli in %s on line %d
done!
