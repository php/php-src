--TEST--
Check that IMAPConnection is declared final
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
--FILE--
<?php

class T extends IMAPConnection {}
--EXPECTF--
Fatal error: Class T may not inherit from final class (IMAPConnection) in %s on line %d
