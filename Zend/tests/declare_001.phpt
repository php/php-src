--TEST--
Testing declare statement with several type values
--FILE--
<?php

declare(encoding = 1);
declare(encoding = 1123131232131312321);
declare(encoding = NULL);
declare(encoding = M_PI);
declare(encoding = 'utf-8');

print 'DONE';

?>
--EXPECT--
DONE
