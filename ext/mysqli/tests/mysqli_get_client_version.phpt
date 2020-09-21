--TEST--
mysqli_get_client_version()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
    if (!is_int($info = mysqli_get_client_version()) || ($info < 100))
        printf("[001] Expecting int/any_non_empty, got %s/%s\n", gettype($info), $info);

    print "done!";
?>
--EXPECT--
done!
