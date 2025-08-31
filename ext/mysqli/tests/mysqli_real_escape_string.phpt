--TEST--
mysqli_real_escape_string()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';

    if ('\\\\' !== ($tmp = mysqli_real_escape_string($link, '\\')))
        printf("[004] Expecting \\\\, got %s\n", $tmp);

    if ('\"' !== ($tmp = mysqli_real_escape_string($link, '"')))
        printf("[005] Expecting \", got %s\n", $tmp);

    if ("\'" !== ($tmp = mysqli_real_escape_string($link, "'")))
        printf("[006] Expecting ', got %s\n", $tmp);

    if ("\\n" !== ($tmp = mysqli_real_escape_string($link, "\n")))
        printf("[007] Expecting \\n, got %s\n", $tmp);

    if ("\\r" !== ($tmp = mysqli_real_escape_string($link, "\r")))
        printf("[008] Expecting \\r, got %s\n", $tmp);

    if ("foo\\0bar" !== ($tmp = mysqli_real_escape_string($link, "foo" . chr(0) . "bar")))
        printf("[009] Expecting %s, got %s\n", "foo\\0bar", $tmp);

    mysqli_close($link);

    try {
        mysqli_real_escape_string($link, 'foo');
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    print "done!";
?>
--EXPECT--
mysqli object is already closed
done!
