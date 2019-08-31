--TEST--
WeakReference serials
--FILE--
<?php
$wr = WeakReference::create(new stdClass);

try {
	serialize($wr);
} catch (Exception $ex) {
	var_dump($ex->getMessage());
}

$wrs = 'O:13:"WeakReference":0:{}';

var_dump(@unserialize($wrs));
?>
--EXPECT--
string(47) "Serialization of 'WeakReference' is not allowed"
bool(false)
