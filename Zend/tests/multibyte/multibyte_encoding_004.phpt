--TEST--
test for mbstring script_encoding for flex unsafe encoding (Shift_JIS)
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
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
