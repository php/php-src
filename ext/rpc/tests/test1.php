<?php
print "huhuhdsa";

$rpc = new com("class", true);
$rpc2 = new com("class", true, "hehe", 12);
$rpc->call("blah");
$rpc->call("heh");
com_call($rpc, "call", 1);
//$rpc->addref();

//$clone = $rpc->__clone();

//$rpc->prop = 1;
//$a = $rpc->prop;

//$a = &$rpc->prop;

//delete $rpc;
//delete $clone;

$heh = com_load(1, 1);
$heh->knorp();
/*delete $heh;*/
?>