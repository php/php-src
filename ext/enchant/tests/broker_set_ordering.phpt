--TEST--
enchant_broker_set_ordering() function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php
if(!extension_loaded('enchant')) die('skip, enchant not loader');
if (!is_object(enchant_broker_init())) {die("skip, resource dont load\n");}
if (!is_array(enchant_broker_list_dicts(enchant_broker_init()))) {die("skip, no dictionary installed on this machine! \n");}
?>
--FILE--
<?php
$broker = enchant_broker_init();
$dicts = enchant_broker_list_dicts($broker);
$comma = ";";

if (is_object($broker)) {
    echo("OK\n");
    if (enchant_broker_set_ordering($broker,$dicts[0]['lang_tag'],$comma)) {
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
