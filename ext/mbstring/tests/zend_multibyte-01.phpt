--TEST--
zend multibyte (1)
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=On
zend.script_encoding=Shift_JIS
internal_encoding=Shift_JIS
--FILE--
<?php
    function �\�\�\($����)
    {
        echo $����;
    }

    �\�\�\("�h���~�t�@�\");
?>
--EXPECT--
�h���~�t�@�\
