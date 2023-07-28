--TEST--
mysqli fetch MySQL functions
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
mysqli_check_skip_test();
?>
--FILE--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";

    $link = default_mysqli_connect();

    if (!$stmt = mysqli_prepare($link, "SELECT md5('bar'), database(), 'foo'"))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    mysqli_stmt_bind_result($stmt, $c0, $c1, $c2);
    mysqli_stmt_execute($stmt);

    mysqli_stmt_fetch($stmt);
    mysqli_stmt_close($stmt);

    $test = array($c0, $c1, $c2);
    if ($c1 !== get_default_database()) {
        echo "Different data\n";
    }

    var_dump($test);
    mysqli_close($link);
    print "done!";
?>
--EXPECTF--
array(3) {
  [0]=>
  string(32) "37b51d194a7513e45b56f6524f2d51f2"
  [1]=>
  string(%d) "%s"
  [2]=>
  string(3) "foo"
}
done!
