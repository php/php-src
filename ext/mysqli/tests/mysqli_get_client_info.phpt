--TEST--
mysqli_get_client_info()
--EXTENSIONS--
mysqli
--FILE--
<?php
if (!is_string($info = mysqli_get_client_info()) || ('' === $info))
    printf("[001] Expecting string/any_non_empty, got %s/%s\n", gettype($info), $info);

print "done!";
?>
--EXPECT--
done!
