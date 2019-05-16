--TEST--
Bug #73594 (dns_get_record() does not populate $additional out parameter - $authns parameter)
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) die("skip test requiring internet connection");

$out = array();
$ret = 0;
exec("dig -tmx php.net +noall +authority 2>/dev/null", $out, $ret);

if ($ret != 0) die("skip dig command is not present or failed to run");

// skip empty and header lines
$out = preg_grep("/^(?!($|;))/", $out);

if (empty($out)) die("skip local resolver does not return authority records");
?>
--FILE--
<?php
$auth = array();
$res = dns_get_record('php.net', DNS_MX, $auth);
// only check $auth if dns_get_record is successful
var_dump(!empty($res) && empty($auth));
?>
--EXPECT--
bool(false)
