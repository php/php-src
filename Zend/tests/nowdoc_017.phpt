--TEST--
Testing nowdoc in default value for property
--FILE--
<?php

class foo {
    public $bar = <<<'EOT'
bar
EOT;
}

print "ok!\n";

?>
--EXPECT--
ok!
