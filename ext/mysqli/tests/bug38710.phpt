--TEST--
Bug #38710 (data leakage because of nonexisting boundary checking in statements)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

$db = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
$qry=$db->stmt_init();
$qry->prepare("SELECT REPEAT('a',100000)");
$qry->execute();
$qry->bind_result($text);
$qry->fetch();
if ($text !== str_repeat('a', 100000)) {
    var_dump(strlen($text));
}
echo "Done";
?>
--EXPECT--
Done
