--TEST--
enchant_dict_suggest() function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php 
if(!extension_loaded('enchant')) die('skip, enchant not loader');
?>
--FILE--
<?php
$broker = enchant_broker_init();
$lang = "en_US";
$sugs = "soong";

if (is_resource($broker)) {
    echo("OK\n");
    $requestDict = enchant_broker_request_dict($broker,$lang);
    
    if ($requestDict) {
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
