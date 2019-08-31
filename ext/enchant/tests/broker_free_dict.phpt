--TEST--
enchant_broker_free_dict(resource $dict); function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php
if(!extension_loaded('enchant')) die('skip, enchant not loader');
if(!is_resource(enchant_broker_init())) {die("skip, resource dont load\n");}
if(!is_array(enchant_broker_list_dicts(enchant_broker_init()))) {die("skip, dont has dictionary install in this machine! \n");}
?>
--FILE--
<?php
$broker = enchant_broker_init();
$dicts = enchant_broker_list_dicts($broker);
$newWord = "iLoveJava";
if (is_resource($broker)) {
    echo("OK\n");
    $requestDict = enchant_broker_request_dict($broker, $dicts[0]['lang_tag']);

    if ($requestDict) {
        echo("OK\n");
        $AddtoPersonalDict = enchant_dict_add_to_personal($requestDict, $newWord);

        if (NULL === $AddtoPersonalDict) {
            var_dump($AddtoPersonalDict);

            if (enchant_broker_free_dict($requestDict)) {
                echo("OK\n");

            } else {
                echo("dict broker free has failed\n");
            }

        } else {
            echo("dict add to personal failed\n");
        }

    } else {
        echo("broker request dict failed\n");
    }

} else {
    echo("broker is not a resource; failed;\n");
}
echo "OK\n";
?>
--EXPECT--
OK
OK
NULL
OK
OK
