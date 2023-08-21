--TEST--
mysqli iterators
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';

    echo "--- Testing default ---\n";
    if (!is_object($res = mysqli_query($link, "SELECT id FROM test ORDER BY id")))
        printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    else {
        foreach ($res as $row) {
            var_dump($row);
        }
        echo "======\n";

        foreach ($res as $row) {
            var_dump($row);
        }
        mysqli_free_result($res);
        try {
            foreach ($res as $row) {
                $row;
            }
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }
    }
    echo "--- Testing USE_RESULT ---\n";
    if (!is_object($res = mysqli_query($link, "SELECT id FROM test ORDER BY id", MYSQLI_USE_RESULT)))
        printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    else {
        foreach ($res as $row) {
            var_dump($row);
        }
        echo "======\n";
        foreach ($res as $row) {
            var_dump($row);
        }
        mysqli_free_result($res);
    }

    echo "--- Testing STORE_RESULT ---\n";
    if (!is_object($res = mysqli_query($link, "SELECT id FROM test ORDER BY id", MYSQLI_STORE_RESULT)))
        printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    else {
        foreach ($res as $row) {
            var_dump($row);
        }
        echo "======\n";
        foreach ($res as $row) {
            var_dump($row);
        }
        mysqli_free_result($res);
    }

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECTF--
--- Testing default ---
array(1) {
  ["id"]=>
  string(1) "1"
}
array(1) {
  ["id"]=>
  string(1) "2"
}
array(1) {
  ["id"]=>
  string(1) "3"
}
array(1) {
  ["id"]=>
  string(1) "4"
}
array(1) {
  ["id"]=>
  string(1) "5"
}
array(1) {
  ["id"]=>
  string(1) "6"
}
======
array(1) {
  ["id"]=>
  string(1) "1"
}
array(1) {
  ["id"]=>
  string(1) "2"
}
array(1) {
  ["id"]=>
  string(1) "3"
}
array(1) {
  ["id"]=>
  string(1) "4"
}
array(1) {
  ["id"]=>
  string(1) "5"
}
array(1) {
  ["id"]=>
  string(1) "6"
}
mysqli_result object is already closed
--- Testing USE_RESULT ---
array(1) {
  ["id"]=>
  string(1) "1"
}
array(1) {
  ["id"]=>
  string(1) "2"
}
array(1) {
  ["id"]=>
  string(1) "3"
}
array(1) {
  ["id"]=>
  string(1) "4"
}
array(1) {
  ["id"]=>
  string(1) "5"
}
array(1) {
  ["id"]=>
  string(1) "6"
}
======

Warning: Data fetched with MYSQLI_USE_RESULT can be iterated only once in %s on line %d
--- Testing STORE_RESULT ---
array(1) {
  ["id"]=>
  string(1) "1"
}
array(1) {
  ["id"]=>
  string(1) "2"
}
array(1) {
  ["id"]=>
  string(1) "3"
}
array(1) {
  ["id"]=>
  string(1) "4"
}
array(1) {
  ["id"]=>
  string(1) "5"
}
array(1) {
  ["id"]=>
  string(1) "6"
}
======
array(1) {
  ["id"]=>
  string(1) "1"
}
array(1) {
  ["id"]=>
  string(1) "2"
}
array(1) {
  ["id"]=>
  string(1) "3"
}
array(1) {
  ["id"]=>
  string(1) "4"
}
array(1) {
  ["id"]=>
  string(1) "5"
}
array(1) {
  ["id"]=>
  string(1) "6"
}
done!
