--TEST--
Check that IMAP\Connection is declared final
--EXTENSIONS--
imap
--FILE--
<?php

class T extends IMAP\Connection {}
--EXPECTF--
Fatal error: Class T may not inherit from final class (IMAP\Connection) in %s on line %d
