--TEST--
Bug #73594 (dns_get_record() does not populate $additional out parameter)
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) die("skip test requiring internet connection");

$out = array();
$ret = 0;
exec("dig -tmx php.net +noall +additional 2>/dev/null", $out, $ret);

if ($ret != 0) die("skip dig command is not present or failed to run");

// skip empty and header lines
$out = preg_grep("/^(?!($|;))/", $out);

if (empty($out)) die("skip local resolver does not return additional records");
?>
--FILE--
<?php
$auth = array();
$additional = array();
dns_get_record('php.net', DNS_MX, $auth, $additional);
var_dump(empty($additional));
?>
--EXPECT--
bool(false)
