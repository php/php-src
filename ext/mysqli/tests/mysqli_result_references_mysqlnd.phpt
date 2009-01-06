--TEST--
References to result sets - mysqlnd (no copies but references)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

require_once('connect.inc');
if (!$IS_MYSQLND)
	die("skip Test for mysqlnd only");
?>
<?php require_once('skipifemb.inc'); ?>
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
		/* enforce seperation */
		$references[$idx]['id_copy_mod']= $rows[$i]['id'] + 0;
	}
	mysqli_free_result($res);

	debug_zval_dump($references);
	print "done!";
?>
--EXPECTF--
array(2) refcount(2){
  [0]=>
  array(4) refcount(1){
    [u"row_ref" { 0072 006f 0077 005f 0072 0065 0066 }]=>
    &NULL refcount(2)
    [u"row_copy" { 0072 006f 0077 005f 0063 006f 0070 0079 }]=>
    array(2) refcount(1){
      [u"id" { 0069 0064 }]=>
      unicode(1) "1" { 0031 } refcount(2)
      [u"label" { 006c 0061 0062 0065 006c }]=>
      unicode(1) "a" { 0061 } refcount(2)
    }
    [u"id_ref" { 0069 0064 005f 0072 0065 0066 }]=>
    unicode(1) "1" { 0031 } refcount(1)
    [u"id_copy" { 0069 0064 005f 0063 006f 0070 0079 }]=>
    unicode(1) "1" { 0031 } refcount(1)
  }
  [1]=>
  array(5) refcount(1){
    [u"row_ref" { 0072 006f 0077 005f 0072 0065 0066 }]=>
    &array(2) refcount(2){
      [u"id" { 0069 0064 }]=>
      &unicode(1) "2" { 0032 } refcount(2)
      [u"label" { 006c 0061 0062 0065 006c }]=>
      unicode(1) "b" { 0062 } refcount(3)
    }
    [u"row_copy" { 0072 006f 0077 005f 0063 006f 0070 0079 }]=>
    array(2) refcount(1){
      [u"id" { 0069 0064 }]=>
      unicode(1) "2" { 0032 } refcount(2)
      [u"label" { 006c 0061 0062 0065 006c }]=>
      unicode(1) "b" { 0062 } refcount(3)
    }
    [u"id_ref" { 0069 0064 005f 0072 0065 0066 }]=>
    &unicode(1) "2" { 0032 } refcount(2)
    [u"id_copy" { 0069 0064 005f 0063 006f 0070 0079 }]=>
    unicode(1) "2" { 0032 } refcount(1)
    [u"id_copy_mod" { 0069 0064 005f 0063 006f 0070 0079 005f 006d 006f 0064 }]=>
    long(2) refcount(1)
  }
}
done!
