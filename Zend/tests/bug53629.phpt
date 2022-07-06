--TEST--
Bug #53629 (memory leak inside highlight_string())
--FILE--
<?php

$str = '
<?php
class foo {
    public $bar = <<<EOT
bar
    EOT;
}
?>
';
$str2 = '
<?php
var_dump(array(<<<EOD
foobar!
EOD
));
?>
';

highlight_string($str, true);
highlight_string($str2, true);


echo "Done\n";
?>
--EXPECT--
Done
