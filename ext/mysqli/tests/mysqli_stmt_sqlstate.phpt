--TEST--
mysqli_stmt_sqlstate()
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

    if (!is_null($tmp = @mysqli_stmt_sqlstate()))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @mysqli_stmt_sqlstate($link)))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    require('table.inc');

    if (!is_null($tmp = @mysqli_stmt_sqlstate($link, '')))
        printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!$stmt = mysqli_stmt_init($link))
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (false !== ($tmp = mysqli_stmt_sqlstate($stmt)))
        printf("[005] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    if (!mysqli_stmt_prepare($stmt, "SELECT id FROM test"))
        printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if ('00000' !== ($tmp = mysqli_stmt_sqlstate($stmt)))
        printf("[007] Expecting string/00000, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if (mysqli_stmt_prepare($stmt, "SELECT believe_me FROM i_dont_belive_that_this_table_exists"))
        printf("[008] Should fail! [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    if ('' === ($tmp = mysqli_stmt_sqlstate($stmt)))
        printf("[009] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    mysqli_stmt_close($stmt);

    if (false !== ($tmp = mysqli_stmt_sqlstate($stmt)))
        printf("[010] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_sqlstate(): invalid object or resource mysqli_stmt
 in %s on line %d

Warning: mysqli_stmt_sqlstate(): Couldn't fetch mysqli_stmt in %s on line %d
done!
