--TEST--
Bug #20381 (array_merge_recursive mangles input arrays)
--FILE--
<?php
$a = array(
	'a1' => 1,
	'a2' => array( 1, 2, 3 ),
	'a3' => array(
		'a' => array( 10, 20, 30 ),
		'b' => 'b'
		)
	);
$b = array( 'a1' => 2,
	'a2' => array( 3, 4, 5 ),
	'a3' => array(
		'c' => 'cc',
		'a' => array( 10, 40 )
		)
	);

var_dump($a);
array_merge_recursive( $a, $b );
var_dump($a);
?>
--EXPECT--
array(3) {
  [u"a1"]=>
  int(1)
  [u"a2"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  [u"a3"]=>
  array(2) {
    [u"a"]=>
    array(3) {
      [0]=>
      int(10)
      [1]=>
      int(20)
      [2]=>
      int(30)
    }
    [u"b"]=>
    unicode(1) "b"
  }
}
array(3) {
  [u"a1"]=>
  int(1)
  [u"a2"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  [u"a3"]=>
  array(2) {
    [u"a"]=>
    array(3) {
      [0]=>
      int(10)
      [1]=>
      int(20)
      [2]=>
      int(30)
    }
    [u"b"]=>
    unicode(1) "b"
  }
}
