--TEST--
Check that IMAP\Connection is declared final
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
--FILE--
<?php

class T extends IMAP\Connection {}
--EXPECTF--
Fatal error: Class T may not inherit from final class (IMAP\Connection) in %s on line %d
