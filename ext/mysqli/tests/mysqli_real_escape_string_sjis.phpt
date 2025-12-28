--TEST--
mysqli_real_escape_string() - sjis
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once __DIR__ . '/test_setup/test_helpers.inc';
$link = mysqli_connect_or_skip();
if (!mysqli_set_charset($link, 'sjis'))
    die(sprintf("skip Cannot set charset 'sjis'"));
?>
--FILE--
<?php
    require_once 'table.inc';

    var_dump(mysqli_set_charset($link, "sjis"));

    if ('?p??\\\\?p??' !== ($tmp = mysqli_real_escape_string($link, '?p??\\?p??')))
        printf("[004] Expecting \\\\, got %s\n", $tmp);

    if ('?p??\"?p??' !== ($tmp = mysqli_real_escape_string($link, '?p??"?p??')))
        printf("[005] Expecting \", got %s\n", $tmp);

    if ("?p??\'?p??" !== ($tmp = mysqli_real_escape_string($link, "?p??'?p??")))
        printf("[006] Expecting ', got %s\n", $tmp);

    if ("?p??\\n?p??" !== ($tmp = mysqli_real_escape_string($link, "?p??\n?p??")))
        printf("[007] Expecting \\n, got %s\n", $tmp);

    if ("?p??\\r?p??" !== ($tmp = mysqli_real_escape_string($link, "?p??\r?p??")))
        printf("[008] Expecting \\r, got %s\n", $tmp);

    if ("?p??\\0?p??" !== ($tmp = mysqli_real_escape_string($link, "?p??" . chr(0) . "?p??")))
        printf("[009] Expecting %s, got %s\n", "?p??\\0?p??", $tmp);

    var_dump(mysqli_query($link, "INSERT INTO test(id, label) VALUES (100, '?p')"));

    mysqli_close($link);
    print "done!";
?>
--EXPECT--
bool(true)
bool(true)
done!
