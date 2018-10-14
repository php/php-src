--TEST--
Bug #52082 (character_set_client & character_set_connection reset after mysqli_change_user)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");
	$link = mysqli_init();
	$link->options(MYSQLI_SET_CHARSET_NAME, "latin2");
	if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
		die("can't connect");
	}
	var_dump($link->query("show variables like 'character_set_client'")->fetch_row());
	var_dump($link->query("show variables like 'character_set_connection'")->fetch_row());
	$link->change_user($user, $passwd, $db);
	var_dump($link->query("show variables like 'character_set_client'")->fetch_row());
	var_dump($link->query("show variables like 'character_set_connection'")->fetch_row());

	print "done!";
?>
--EXPECTF--
array(2) {
  [0]=>
  string(20) "character_set_client"
  [1]=>
  string(6) "latin2"
}
array(2) {
  [0]=>
  string(24) "character_set_connection"
  [1]=>
  string(6) "latin2"
}
array(2) {
  [0]=>
  string(20) "character_set_client"
  [1]=>
  string(6) "latin2"
}
array(2) {
  [0]=>
  string(24) "character_set_connection"
  [1]=>
  string(6) "latin2"
}
done!
