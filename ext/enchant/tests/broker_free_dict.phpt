--TEST--
enchant_broker_free_dict(resource $dict); function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--EXTENSIONS--
enchant
--SKIPIF--
<?php
if(!is_object(enchant_broker_init())) {die("skip, resource dont load\n");}
if(!is_array(enchant_broker_list_dicts(enchant_broker_init()))) {die("skip, no dictionary installed on this machine! \n");}
?>
--FILE--
<?php
$broker = enchant_broker_init();
$dicts = enchant_broker_list_dicts($broker);
$newWord = "iLoveJava";
if (is_object($broker)) {
    echo("OK\n");
    $requestDict = enchant_broker_request_dict($broker, $dicts[0]['lang_tag']);

    if ($requestDict) {
        echo("OK\n");
        $AddtoPersonalDict = enchant_dict_add($requestDict, $newWord);

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
--EXPECTF--
OK
OK
NULL

Deprecated: Function enchant_broker_free_dict() is deprecated in %s
OK
OK
