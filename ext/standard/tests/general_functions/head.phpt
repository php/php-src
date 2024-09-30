--TEST--
header() and friends
--FILE--
<?php
$sent1 = headers_sent();
$list1 = headers_list();

header("HTTP 1.1", true, 200);
$list2 = headers_list();
header("Date: Mon, 25 Mar 1985 00:20:00 GMT");
$list3 = headers_list();

header_remove();
$list4 = headers_list();

var_dump(
  $sent1,
  $list1,
  $list2,
  $list3,
  $list4,
);

header("Too late !!");

echo "Done\n";
?>
--EXPECTF--
bool(false)
array(1) {
  [0]=>
  string(%d) "X-Powered-By: PHP/%s"
}
array(2) {
  [0]=>
  string(%d) "X-Powered-By: PHP/%s"
  [1]=>
  string(8) "HTTP 1.1"
}
array(3) {
  [0]=>
  string(%d) "X-Powered-By: PHP/%s"
  [1]=>
  string(8) "HTTP 1.1"
  [2]=>
  string(35) "Date: Mon, 25 Mar 1985 00:20:00 GMT"
}
array(0) {
}

Warning: Cannot modify header information - headers already sent by (output started at %s:%d) in %s on line %d
Done
