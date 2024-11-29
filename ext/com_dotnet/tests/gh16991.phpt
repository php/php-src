--TEST--
GH-16991 (Getting typeinfo of non DISPATCH variant segfaults)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
com_print_typeinfo(new variant("hello"));
?>
--EXPECTF--
Warning: com_print_typeinfo(): Unable to find typeinfo using the parameters supplied in %s on line %d
