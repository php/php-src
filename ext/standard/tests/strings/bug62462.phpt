--TEST--
Multibyte characters shouldn't be split by soft line break added by quoted_printable_encode - 4 byte character test
--FILE--
<?php
echo quoted_printable_encode(str_repeat("\xc4\x85", 77));
?>

==DONE==
--EXPECT--
=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=
=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=
=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=
=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=
=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=
=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=C4=85=
=C4=85=C4=85=C4=85=C4=85=C4=85
==DONE==
