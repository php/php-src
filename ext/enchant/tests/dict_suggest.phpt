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
$lang = "en_EN";
$word = "aspell";

if (is_resource($broker)) {
    echo("OK\n");
    $requestDict = enchant_broker_request_dict($broker,$lang);
    
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
