--TEST--
mysqli_stmt_attr_get() - prefetch
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
die("SKIP: prefetch isn't supported at the moment");
?>
--FILE--
<?php
    require('table.inc');

    $stmt = mysqli_stmt_init($link);
    mysqli_stmt_prepare($stmt, 'SELECT * FROM test');
    if (1 !== ($tmp = mysqli_stmt_attr_get($stmt, MYSQLI_STMT_ATTR_PREFETCH_ROWS))) {
        printf("[001] Expecting int/1, got %s/%s for attribute %s/%s\n",
            gettype($tmp), $tmp, $k, $attr);
    }
    $stmt->close();
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
done!
