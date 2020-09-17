--TEST--
mysqli_real_escape_string() - SQL Mode NO_BACKSLASH_ESCAPE
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");
    require_once('table.inc');

    if (!mysqli_query($link, 'SET @@sql_mode="NO_BACKSLASH_ESCAPES"'))
        printf("[001] Cannot set NO_BACKSLASH_ESCAPES, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ('\\' !== ($tmp = mysqli_real_escape_string($link, '\\')))
        printf("[002] Expecting \\, got %s\n", $tmp);

    if ('"' !== ($tmp = mysqli_real_escape_string($link, '"')))
        printf("[003] Expecting \", got %s\n", $tmp);

    if ("''" !== ($tmp = mysqli_real_escape_string($link, "'")))
        printf("[004] Expecting '', got %s\n", $tmp);

    if ("\n" !== ($tmp = mysqli_real_escape_string($link, "\n")))
        printf("[005] Expecting \\n, got %s\n", $tmp);

    if ("\r" !== ($tmp = mysqli_real_escape_string($link, "\r")))
        printf("[006] Expecting \\r, got %s\n", $tmp);

    assert("foo" . chr(0) . "bar" === "foo" . chr(0) . "bar");
    if ("foo" . chr(0) . "bar" !== ($tmp = mysqli_real_escape_string($link, "foo" . chr(0) . "bar")))
        printf("[007] Expecting %s, got %s\n", "foo" . chr(0) . "bar", $tmp);

    if (!mysqli_query($link, sprintf('INSERT INTO test(id, label) VALUES (100, "%s")',
            mysqli_real_escape_string($link, "\\"))))
        printf("[009] Cannot INSERT, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!($res = mysqli_query($link, 'SELECT label FROM test WHERE id = 100')) ||
            !($row = mysqli_fetch_assoc($res)))
        printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    var_dump($row);
    mysqli_free_result($res);

    if (!mysqli_query($link, 'SET @@sql_mode=""'))
        printf("[011] Cannot disable NO_BACKSLASH_ESCAPES, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ('\\\\' !== ($tmp = mysqli_real_escape_string($link, '\\')))
        printf("[012] Expecting \\, got %s\n", $tmp);

    if ("foo\\0bar" !== ($tmp = mysqli_real_escape_string($link, "foo" . chr(0) . "bar")))
        printf("[013] Expecting %s, got %s\n", "foo" . chr(0) . "bar", $tmp);

    mysqli_close($link);

    print "done!";
?>
--EXPECT--
array(1) {
  ["label"]=>
  string(1) "\"
}
done!
