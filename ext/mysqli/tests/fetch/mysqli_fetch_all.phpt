--TEST--
mysqli_fetch_all()
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

$link = setup_table_with_data_on_default_connection('test_mysqli_fetch_all');

$result = mysqli_query($link, "SELECT * FROM test_mysqli_fetch_all ORDER BY id LIMIT 2");

echo "Default mode\n";
var_dump(mysqli_fetch_all($result));
mysqli_free_result($result);

$result = mysqli_query($link, "SELECT * FROM test_mysqli_fetch_all ORDER BY id LIMIT 2");
print "Mode: MYSQLI_NUM\n";
var_dump(mysqli_fetch_all($result, MYSQLI_NUM));
mysqli_free_result($result);

$result = mysqli_query($link, "SELECT * FROM test_mysqli_fetch_all ORDER BY id LIMIT 2");
print "Mode: MYSQLI_BOTH\n";
var_dump(mysqli_fetch_all($result, MYSQLI_BOTH));
mysqli_free_result($result);

$result = mysqli_query($link, "SELECT * FROM test_mysqli_fetch_all ORDER BY id LIMIT 2");
print "Mode: MYSQLI_ASSOC\n";
var_dump(mysqli_fetch_all($result, MYSQLI_ASSOC));

echo "mysqli_fetch_all() after fetching\n";
var_dump(mysqli_fetch_all($result));
mysqli_free_result($result);

$result = mysqli_query($link, "SELECT 1 AS a, 2 AS a, 3 AS c, 4 AS C, NULL AS d, true AS e");
print "[017]\n";
var_dump(mysqli_fetch_all($result, MYSQLI_BOTH));

// Illegal mode
try {
    mysqli_fetch_all($result, -10);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
mysqli_free_result($result);
try {
    mysqli_fetch_array($result, MYSQLI_ASSOC);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_mysqli_fetch_all');
?>
--EXPECT--
Default mode
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    string(1) "b"
  }
}
Mode: MYSQLI_NUM
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    string(1) "b"
  }
}
Mode: MYSQLI_BOTH
array(2) {
  [0]=>
  array(4) {
    [0]=>
    string(1) "1"
    ["id"]=>
    string(1) "1"
    [1]=>
    string(1) "a"
    ["label"]=>
    string(1) "a"
  }
  [1]=>
  array(4) {
    [0]=>
    string(1) "2"
    ["id"]=>
    string(1) "2"
    [1]=>
    string(1) "b"
    ["label"]=>
    string(1) "b"
  }
}
Mode: MYSQLI_ASSOC
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "2"
    ["label"]=>
    string(1) "b"
  }
}
mysqli_fetch_all() after fetching
array(0) {
}
[017]
array(1) {
  [0]=>
  array(11) {
    [0]=>
    string(1) "1"
    ["a"]=>
    string(1) "2"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
    ["c"]=>
    string(1) "3"
    [3]=>
    string(1) "4"
    ["C"]=>
    string(1) "4"
    [4]=>
    NULL
    ["d"]=>
    NULL
    [5]=>
    string(1) "1"
    ["e"]=>
    string(1) "1"
  }
}
mysqli_fetch_all(): Argument #2 ($mode) must be one of MYSQLI_NUM, MYSQLI_ASSOC, or MYSQLI_BOTH
mysqli_result object is already closed
done!
