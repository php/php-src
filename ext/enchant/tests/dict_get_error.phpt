--TEST--
enchant_dict_get_error() function
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

if (is_resource($broker)) {
    echo("OK\n");
    
    $requestDict = enchant_broker_request_dict($broker,$lang);
    
    if ($requestDict) {
        var_dump(enchant_dict_get_error($requestDict));    
        
    } else {
	echo("broker request dict failed\n");        
    
    }    
   
} else {
    echo("init failed\n");
}
echo "OK\n";
?>
--EXPECT--
OK
bool(false)
OK
