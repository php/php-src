--TEST--
Bug #71914 (Reference is lost in "switch")
--FILE--
<?php

function bug(&$value) {
	switch ($value) {
	case "xxxx":
		$value = true;
		break;
	}
}


function test($arr, &$dummy) {
	bug($arr["str"]);
	var_dump($arr["str"]);
}


$array = array("str" => "xxxx");
test($array, $array["str"]);
?>
--EXPECT--
bool(true)
