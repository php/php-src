--TEST--
mysqli_fetch_object() - calling constructor on class wo constructor
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	require('table.inc');
	if (!$res = mysqli_query($link, "SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 5")) {
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	class mysqli_fetch_object_test {

		public $a = null;
		public $b = null;

		public function toString() {
			var_dump($this);
		}
	}

	printf("No exception with PHP:\n");
	var_dump($obj = new mysqli_fetch_object_test(1, 2));

	printf("\nException with mysqli. Note that at all other places we throws errors but no exceptions unless the error mode has been changed:\n");
	try {
		var_dump($obj = mysqli_fetch_object($res, 'mysqli_fetch_object_test', array(1, 2)));
	} catch (Exception $e) {
		printf("Exception: %s\n", $e->getMessage());
	}

	printf("\nFatal error with PHP (but no exception!):\n");
	var_dump($obj->mysqli_fetch_object_test(1, 2));

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
No exception with PHP:
object(mysqli_fetch_object_test)#%d (%d) {
  [%u|b%"a"]=>
  NULL
  [%u|b%"b"]=>
  NULL
}

Exception with mysqli. Note that at all other places we throws errors but no exceptions unless the error mode has been changed:
Exception: Class mysqli_fetch_object_test does not have a constructor hence you cannot use ctor_params

Fatal error with PHP (but no exception!):

Fatal error: Uncaught Error: Call to undefined method mysqli_fetch_object_test::mysqli_fetch_object_test() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
