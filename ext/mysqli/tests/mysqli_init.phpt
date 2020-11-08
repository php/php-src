--TEST--
mysqli_init()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    $link = mysqli_init();

    if (!is_object($link) && false !== $link)
        printf("[001] Expecting object/mysqli_link or boolean/false, got %s/%s\n", gettype($link), $link);

    if (is_object($link) && 'mysqli' != get_class($link))
        printf("[002] Expecting object of type mysqli got object of type %s\n", get_class($link));

    if ($link)
        mysqli_close($link);

    print "done!";
?>
--EXPECT--
done!
