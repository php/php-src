--TEST--
mysqli_thread_safe()
--EXTENSIONS--
mysqli
--FILE--
<?php
    if (!is_bool($tmp = mysqli_thread_safe()))
        printf("[001] Expecting boolean/any, got %s/%s.\n", gettype($tmp), $tmp);

    print "done!";
?>
--EXPECT--
done!
