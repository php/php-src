<?php
print "huhuhdsa";

$rpc = new com("class", true, "huh");
$rpc2 = new com("class", true, "hehe", 1);
$rpc->call("blah");
$rpc->call("heh");
$rpc->call("blah");
com_call($rpc, 1, "1");
com_call($rpc, 1, "1");
$rpc->call("blah");
$rpc->call("blah");
$rpc->call("blah");
$rpc2->call("hehe");
$rpc2->call("hehe");
$rpc2->call("hehe");
$rpc2->call("hehe");
$rpc2->call("hehe");
com_call($rpc, "call", 1);
com_call($rpc, 1, "1");
com_call($rpc, "call", 1);
com_call($rpc2, "call", 1);
com_call($rpc2, "call", 1);
//$rpc->addref();

//$clone = $rpc->__clone();

//$rpc->prop = 1;
//$a = $rpc->prop;

//$a = &$rpc->prop;

//delete $rpc;
//delete $clone;

$heh = com_load("heh", 1);
$heh->knorp();
/*delete $heh;*/
?>