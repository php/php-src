--TEST--
enchant_dict_add_to_session() function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php 
if(!extension_loaded('enchant')) die('skip, enchant not loader');
?>
--FILE--
<?php
$broker = enchant_broker_init();
$lang = "en_EN";
$newWord = "iLoveJavaScript";

if (is_resource($broker)) {
    echo("OK\n");
    $requestDict = enchant_broker_request_dict($broker,$lang);
    
    if ($requestDict) {
        echo("OK\n");
        $AddtoSessionDict = enchant_dict_add_to_session($requestDict,$newWord);    
        
        if (NULL === $AddtoSessionDict) {
            var_dump($AddtoSessionDict);
        } else {
            echo("dict add to session failed\n");
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
