<?php

echo "pooling test\n";

/* pooling test */
$rpc = new com("pooling", true, 1);
com_poolable($rpc);
delete $rpc;

$rpc = new com("pooling", true, 1);
delete $rpc;

$rpc = new com("pooling", true, 1);
delete $rpc;

$rpc = new com("pooling", true, 1);
delete $rpc;
?>
