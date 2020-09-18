--TEST--
References to result sets - mysqlnd (no copies but references)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');
if (!$IS_MYSQLND)
    die("skip Test for mysqlnd only");
?>
--FILE--
<?php
    require_once('connect.inc');
    require_once('table.inc');

    $references = array();

    if (!(mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id ASC LIMIT 1")) ||
            !($res = mysqli_store_result($link)))
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $idx = 0;
    while ($row = mysqli_fetch_assoc($res)) {
        /* will overwrite itself */
        $references[$idx]['row_ref'] 		= &$row;
        $references[$idx]['row_copy'] 	= $row;
        $references[$idx]['id_ref'] 		= &$row['id'];
        $references[$idx++]['id_copy']	= $row['id'];
    }

    debug_zval_dump($references);
    mysqli_free_result($res);

    if (!(mysqli_real_query($link, "SELECT id, label FROM test ORDER BY id ASC LIMIT 2")) ||
            !($res = mysqli_use_result($link)))
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $rows = array();
    for ($i = 0; $i < 2; $i++) {
        $rows[$i] = mysqli_fetch_assoc($res);
        $references[$idx]['row_ref'] 		= &$rows[$i];
        $references[$idx]['row_copy'] 	= $rows[$i];
        $references[$idx]['id_ref'] 		= &$rows[$i]['id'];
        $references[$idx]['id_copy']		= $rows[$i]['id'];
        /* enforce separation */
        $references[$idx]['id_copy_mod']= $rows[$i]['id'] + 0;
    }
    mysqli_free_result($res);

    debug_zval_dump($references);
    print "done!";
?>
--EXPECTF--
array(1) refcount(%d){
  [0]=>
  array(4) refcount(%d){
    ["row_ref"]=>
    &NULL
    ["row_copy"]=>
    array(2) refcount(1){
      ["id"]=>
      string(1) "1" refcount(%d)
      ["label"]=>
      string(1) "a" refcount(%d)
    }
    ["id_ref"]=>
    string(1) "1" refcount(%d)
    ["id_copy"]=>
    string(1) "1" refcount(%d)
  }
}
array(2) refcount(%d){
  [0]=>
  array(4) refcount(%d){
    ["row_ref"]=>
    &NULL
    ["row_copy"]=>
    array(2) refcount(%d){
      ["id"]=>
      string(1) "1" refcount(%d)
      ["label"]=>
      string(1) "a" refcount(%d)
    }
    ["id_ref"]=>
    string(1) "1" refcount(%d)
    ["id_copy"]=>
    string(1) "1" refcount(%d)
  }
  [1]=>
  array(5) refcount(%d){
    ["row_ref"]=>
    &array(2) refcount(%d){
      ["id"]=>
      &string(1) "2" refcount(%d)
      ["label"]=>
      string(1) "b" refcount(%d)
    }
    ["row_copy"]=>
    array(2) refcount(%d){
      ["id"]=>
      string(1) "2" refcount(%d)
      ["label"]=>
      string(1) "b" refcount(%d)
    }
    ["id_ref"]=>
    &string(1) "2" refcount(%d)
    ["id_copy"]=>
    string(1) "2" refcount(%d)
    ["id_copy_mod"]=>
    int(2)
  }
}
done!
