--TEST--
encoding conversion from script encoding into internal encoding
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
mbstring.encoding_translation = On
zend.script_encoding=Shift_JIS
internal_encoding=UTF-8
--FILE--
<?php
    function —\Ž\”\($ˆø”)
    {
        echo $ˆø”;
    }

    —\Ž\”\("ƒhƒŒƒ~ƒtƒ@ƒ\");
?>
--EXPECT--
ãƒ‰ãƒ¬ãƒŸãƒ•ã‚¡ã‚½
