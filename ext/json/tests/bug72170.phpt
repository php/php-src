--TEST--
Bug #72170 (JsonSerializable may inc apply count without dec it)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

class Dummy implements JsonSerializable{
	public function jsonSerialize() {
		global $flag;
		if ($flag) {
			exit;
		} else {
			return "okey";
		}
	}
}

$array = array();
$array[] = new Dummy;

register_shutdown_function(function() use($array) {
	global $flag;
	$flag = 0;
	var_dump(json_encode(array($array)));
});

$flag = 1;
json_encode(array($array));
?>
--EXPECT--
string(10) "[["okey"]]"
