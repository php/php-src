<?php

echo "hash test\n";

/* hash test */
$rpc1 = new com("hash", true, 1);
$rpc2 = new com("hash", false, 2);
$rpc3 = new com("hash", true, 3);
$rpc4 = com_load("hash", false, 4);
$rpc5 = com_load("hash", true, 5);

$rpc1->{3} = "hh";
com_set($rpc2, "hehe", 3);

$rpc1->call("blah");
$rpc2->call("blah");
$rpc3->call("blah");
$rpc4->call("heh");
$rpc5->call("blah");
?>