--TEST--
enchant_broker_request_dict() function
--SKIPIF--
<?php 
if(!extension_loaded('enchant')) die('skip, enchant not loader');
?>
--FILE--
<?php
$broker = enchant_broker_init();
if (!is_resource($broker)) {
	exit("init failed\n");
}

$dicts = enchant_broker_list_dicts($broker);
if (is_array($dicts)) {
	if (count($dicts)) {
		$dict = enchant_broker_request_dict($broker, $dicts[0]['lang_tag']);
		if (is_resource($dict)) {
			echo "OK\n";
		} else {
			echo "fail to request " . $dicts[0]['lang_tag'];
		}
	}
} else {
	exit("list dicts failed\n");
}
echo "OK\n";
?>
--EXPECT--
OK
OK
