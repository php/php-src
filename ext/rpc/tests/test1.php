<?php
print "huhuhdsa";

$rpc = new com();
$rpc->call();
$rpc->addref();

$clone = $rpc->__clone();

//$rpc->prop = 1;
//$a = $rpc->prop;

$a = &$rpc->prop;

delete $rpc;
delete $clone;

$heh = com_load();
/*$heh->call;
delete $heh;*/
?>