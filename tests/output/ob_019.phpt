--TEST--
output buffering - flags
--FILE--
<?php
declare(encoding="latin1");

echo "\n==".ob_get_level()."==\n";
ob_start(null, 0, PHP_OUTPUT_HANDLER_CLEANABLE);
echo "N:clean\n";
ob_clean();
ob_flush();

echo "\n==".ob_get_level()."==\n";
ob_start(null, 0, PHP_OUTPUT_HANDLER_FLUSHABLE);
echo "Y:flush\n";
ob_clean();
ob_flush();

echo "\n==".ob_get_level()."==\n";
ob_start(null, 0, PHP_OUTPUT_HANDLER_REMOVABLE);
echo "N:remove-clean\n";
ob_end_clean();

echo "\n==".ob_get_level()."==\n";
ob_start(null, 0, PHP_OUTPUT_HANDLER_REMOVABLE);
echo "Y:remove-flush\n";
ob_end_flush();

echo "\n==".ob_get_level()."==\n";
ob_start(null, 0, PHP_OUTPUT_HANDLER_STDFLAGS);
echo "N:standard\n";
ob_end_clean();

?>
--EXPECTF--

==0==

Notice: ob_flush(): failed to flush buffer of default output handler (0) in %sob_019.php on line %d

==1==
Y:flush

Notice: ob_clean(): failed to delete buffer of default output handler (1) in %sob_019.php on line %d

==2==

==2==
Y:remove-flush

==2==
