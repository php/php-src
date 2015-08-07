--TEST--
enchant_broker_list_dicts() function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php 
if(!extension_loaded('enchant')) die('skip, enchant not loader');
?>
--FILE--
<?php
$broker = enchant_broker_init();
if (is_resource($broker)) {
    echo("OK\n");
    $brokerListDicts = enchant_broker_list_dicts($broker));
    
    if (is_array($brokerListDicts)) {
        echo("OK\n");        
    } else {
        exit("broker list dicts failed\n");        
    }    
} else {
    exit("init failed\n");
}

?>
--EXPECT--
OK
OK
