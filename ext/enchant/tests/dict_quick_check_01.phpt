--TEST--
enchant_dict_quick_check() function
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
$word = "aspell";

if (is_object($broker)) {
    echo("OK\n");
    $requestDict = enchant_broker_request_dict($broker, $dicts[0]['lang_tag']);

    if ($requestDict) {
        enchant_dict_quick_check($requestDict,$word,$sugs);

        if (is_array($sugs)) {
            echo("OK\n");
        } else {
            echo("dict quick check failed\n");
        }
    } else {
        echo("broker request dict failed\n");
    }
} else {
    echo("broker is not a resource; failed;\n");
}
?>
--EXPECT--
OK
OK
