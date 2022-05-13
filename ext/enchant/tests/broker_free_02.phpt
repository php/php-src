--TEST--
@enchant_broker_free() function
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
$newWord = array("iLoveJava","iLoveJavascript","iLoveRuby","iLovePerl","iLoveAwk","iLoveC");

if (is_object($broker)) {
    echo("OK\n");
    $requestDict = enchant_broker_request_dict($broker, $dicts[0]['lang_tag']);

    if ($requestDict) {
        echo("OK\n");
        for($x=0;$x<count($newWord);$x++) {
            $AddtoPersonalDict = enchant_dict_add($requestDict,$newWord[$x]);
        }

        if (NULL === $AddtoPersonalDict) {
            var_dump($AddtoPersonalDict);

            if (@enchant_broker_free_dict($requestDict)) {
                echo("OK\n");

                if (@enchant_broker_free($broker)) {
                    echo("OK\n");

                } else {
                    echo("broker free failed\n");
                }
            } else {
                echo("broker dict free failed\n");
            }
        } else {
            echo("dict add to personal failed\n");
        }
    } else {
        echo("broker request dict failed\n");
    }
} else {
    echo("init failed\n");
}
echo("OK\n");
?>
--EXPECT--
OK
OK
NULL
OK
OK
OK
