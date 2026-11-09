--TEST--
GH-25284 (quoted_printable_encode splits UTF-8 characters)
--CREDITS--
matthijskooijman
--FILE--
<?php
echo quoted_printable_encode("X" . str_repeat("\xc4\x85", 15)), "\n";
echo quoted_printable_encode("X" . str_repeat("\xe2\x82\xac", 10)), "\n";
echo quoted_printable_encode(str_repeat("\xfc", 40)), "\n";
?>
--EXPECT--
X=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=
=C4=85=C4=85=C4=85
X=E2=82=AC=E2=82=AC=E2=82=AC=E2=82=AC=E2=82=AC=E2=82=AC=E2=82=AC=E2=82=AC=
=E2=82=AC=E2=82=AC
=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=
=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC=FC
