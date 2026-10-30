--TEST--
GH-18113 (stack-buffer-overflow ext/opcache/jit/ir/ir_sccp.c)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1205
--FILE--
<?php
function lookup($s){
switch($fusion){
case 1: return 1;
case 4: return 4;
case 5: return 5;
case 14: return 14;
case 15: return 15;
case 488: return 488;
case 489: return 489;
case 490: return 490;
case 491: return 491;
case 492: return 492;
case 493: return 493;
case 494: return 494;
case 495: return 495;
case 496: return 496;
case 497: return 497;
case 498: return 498;
case 499: return 499;
case 500: return 500;
case 501: return 501;
case 502: return 502;
case 503: return 503;
case 504: return 504;
case 505: return 505;
case 506: return 506;
case 507: return 507;
case 508: return 508;
case 509: return 509;
case 510: return 510;
case 511: return 511;
case 512: return 512;
case 513: return 513;
};
}
echo "Done\n";
?>
--EXPECT--
Done
