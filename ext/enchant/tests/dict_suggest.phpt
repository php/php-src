--TEST--
enchant_dict_suggest() function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php
if(!extension_loaded('enchant')) die('skip, enchant not loader');
if (!is_resource(enchant_broker_init())) {die("skip, resource dont load\n");}
if (!is_array(enchant_broker_list_dicts(enchant_broker_init()))) {die("skip, dont has dictionary install in this machine! \n");}
?>
--FILE--
<?php
$broker = enchant_broker_init();
$dicts = enchant_broker_list_dicts($broker);
$sugs = "soong";

if (is_resource($broker)) {
    echo("OK\n");
    $requestDict = enchant_broker_request_dict($broker, $dicts[0]['lang_tag']);

    if ($requestDict) {
    	echo("OK\n");
        $dictSuggest = enchant_dict_suggest($requestDict,$sugs);

        if (is_array($dictSuggest)) {
            echo("OK\n");
        } else {
            echo("dict suggest failed\n");
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
OK
