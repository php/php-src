--TEST--
Heredocs can NOT be used as static scalars.
--FILE--
<?php

require_once 'nowdoc.inc';

class e {
    
    const E = <<<THISMUSTERROR
If you see this, something's wrong.
THISMUSTERROR;

};

print e::E . "\n";

?>
--EXPECTF--
Parse error: syntax error, unexpected T_START_HEREDOC in %sheredoc_011.php on line 8
