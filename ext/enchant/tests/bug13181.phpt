--TEST--
bug #13181, leaving a context frees the broker resources
--SKIPIF--
<?php 
if(!extension_loaded('enchant')) die('skip, enchant not loader');

 ?>
--FILE--
<?php
function get_dictionnary() {
    $rBroker = enchant_broker_init();
	$t = enchant_broker_request_dict($rBroker, 'en');
	var_dump($t);
    return $t;
}
$rDict = get_dictionnary();
var_dump($rDict);
enchant_dict_suggest($rDict, "soong");

function get_broker() {
    $t = enchant_broker_init();
	var_dump($t);
    return $t;
}

$rbroker = get_broker();
var_dump($rbroker);

function get_dict($broker) {
	$t = enchant_broker_request_dict($broker, 'en');
	var_dump($t);
    return $t;
}
$rDict = get_dict($rbroker);
var_dump($rDict);
?>
--EXPECTF--
resource(%d) of type (enchant_dict)
resource(%d) of type (enchant_dict)
resource(%d) of type (enchant_broker)
resource(%d) of type (enchant_broker)
resource(%d) of type (enchant_dict)
resource(%d) of type (enchant_dict)
