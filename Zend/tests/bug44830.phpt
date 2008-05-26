--TEST--
Bug #44830 (Very minor issue with backslash in heredoc)
--FILE--
<?php

$backslash = <<<EOT
\
EOT;

var_dump($backslash);

?>
--EXPECT--
unicode(1) "\"
