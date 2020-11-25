--TEST--
mysqli_thread_id()
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

    if (!is_null($tmp = @mysqli_thread_id()))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    if (!is_null($tmp = @mysqli_thread_id($link)))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    require('table.inc');

    if (!is_int($tmp = mysqli_thread_id($link)) || (0 === $tmp))
        printf("[003] Expecting int/any but zero, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysqli_errno($link), mysqli_error($link));

    // should work if the thread id is correct
    mysqli_kill($link, mysqli_thread_id($link));

    mysqli_close($link);

    if (false !== ($tmp = mysqli_thread_id($link)))
        printf("[005] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_thread_id(): Couldn't fetch mysqli in %s on line %d
done!
