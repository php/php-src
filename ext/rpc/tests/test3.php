<?php

echo "singleton test\n";

/* singleton test */
$rpc1 = new com("singleton", true, 1);
com_singleton($rpc1);

$rpc2 = new com("singleton", false, 2);
$rpc3 = new com("singleton", true, 3);
$rpc4 = new com("singleton", false, 4);
$rpc5 = new com("singleton", true, 5);

delete $rpc1;
delete $rpc2;
delete $rpc3;
delete $rpc4;
delete $rpc5;
?>
