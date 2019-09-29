--TEST--
Fail gracefully on empty result set
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");
	require('table.inc');

	// Returns only one result set
	$link->multi_query("SELECT 1");
    var_dump($link->next_result()); // should return false
    var_dump($link->store_result()); // now what happens here!?


	// Returns only one result set
	$link->multi_query("SELECT 1");
    var_dump($link->next_result());
    var_dump($link->use_result());

    $link->close();
?>
=== DONE ===
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECT--
bool(false)
object(mysqli_result)#3 (5) {
  ["current_field"]=>
  int(0)
  ["field_count"]=>
  int(1)
  ["lengths"]=>
  NULL
  ["num_rows"]=>
  int(1)
  ["type"]=>
  int(0)
}
bool(false)
object(mysqli_result)#3 (5) {
  ["current_field"]=>
  int(0)
  ["field_count"]=>
  int(1)
  ["lengths"]=>
  NULL
  ["num_rows"]=>
  int(0)
  ["type"]=>
  int(1)
}
=== DONE ===
