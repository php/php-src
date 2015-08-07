--TEST--
enchant_dict_check() function
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
$newWord = "java";

if (is_resource($broker)) {
    echo("OK\n");
    $requestDict = enchant_broker_request_dict($broker,$lang);
    
    if ($requestDict) {
        echo("OK\n");
      
        if (enchant_dict_check($requestDict,$newWord)) {
            echo("OK\n");
            
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
OK
OK
