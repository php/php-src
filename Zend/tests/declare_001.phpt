--TEST--
Testing declare statement with several type values
--FILE--
<?php

declare(encoding = 1);
declare(encoding = 1.2);
declare(encoding = NULL);
declare(encoding = M_PI);
declare(encoding = 'utf-8');

print 'DONE';

?>
--EXPECTF--
Warning: Unsupported encoding [1] in %s on line %d

Warning: Unsupported encoding [1.2] in %s on line %d

Warning: Unsupported encoding [] in %s on line %d

Fatal error: Cannot use constants as encoding in %s on line %d
