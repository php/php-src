--TEST--
Bug #64879: quoted_printable_encode() wrong size calculation (CVE-2013-2110)
--FILE--
<?php

quoted_printable_encode(str_repeat("\xf4", 1000));
quoted_printable_encode(str_repeat("\xf4", 100000));

echo "Done\n";
?>
--EXPECT--
Done
