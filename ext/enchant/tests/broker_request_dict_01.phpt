--TEST--
enchant_broker_request_dict() function
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

if (is_array($dicts)) {
        $dict = enchant_broker_request_dict($broker, $dicts[0]['lang_tag']);
        if (is_object($dict)) {
            echo("OK\n");
        } else {
            echo("fail to request " . $dicts[0]['lang_tag']);
        }
} else {
    echo("list dicts failed\n");
}
echo("OK\n");
?>
--EXPECT--
OK
OK
