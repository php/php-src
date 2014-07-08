--TEST--
Bug #67052 - NumberFormatter::parse() resets LC_NUMERIC setting
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

function ut_main()
{
        setlocale(LC_ALL, 'de_DE');
        $fmt = new NumberFormatter( 'sl_SI.UTF-8', NumberFormatter::DECIMAL);
        $num = "1.234.567,891";
        $res_str =  $fmt->parse($num)."\n";
        $res_str .=  setlocale(LC_NUMERIC, 0);
        return $res_str;
}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
1234567,891
de_DE

