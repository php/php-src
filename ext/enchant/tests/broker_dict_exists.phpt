--TEST--
enchant_broker_dict_exists() function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php 
if(!extension_loaded('enchant')) die('skip, enchant not loader');
?>
--FILE--
<?php
$lang="en_US";
$broker = enchant_broker_init();
if (!is_resource($broker)) {
    exit("init failed\n");
}

if (enchant_broker_dict_exists($broker,$lang)) {
    echo("OK\n");
} else {
    echo("dicts dont exist failed\n");
}
?>
--EXPECT--
OK
