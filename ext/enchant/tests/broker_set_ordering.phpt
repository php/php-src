--TEST--
enchant_broker_set_ordering() function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php 
if(!extension_loaded('enchant')) die('skip, enchant not loader');
?>
--FILE--
<?php
$broker = enchant_broker_init();
$lang = "us_US";
$comma = ";";

if (is_resource($broker)) {
    echo("OK\n");
    if (enchant_broker_set_ordering($broker,$lang,$comma))  {
        echo("OK\n");
    } else {
        echo("enchant failed ==>" . $enchantErr);
    }
} else {
    echo("init failed\n");
}
echo "OK\n";
?>
--EXPECT--
OK
OK
OK
