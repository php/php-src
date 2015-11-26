--TEST--
Testing mb_ereg_search() function
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring not enabled');
function_exists('mb_ereg_search') or die("skip mb_ereg_search() is not available in this build");
?>
--FILE--
<?php
    $str = "中国abc + abc ?!？！字符＃　china string";

    $reg = "\w+";

    mb_regex_encoding("UTF-8");

    mb_ereg_search_init($str, $reg);
    $r = mb_ereg_search();

    if(!$r)
    {
        echo "null\n";
    }
    else
    {
        $r = mb_ereg_search_getregs(); //get first result
        do
        {
            var_dump($r[0]);
            $r = mb_ereg_search_regs();//get next result
        }
        while($r);
    }
?>
--EXPECT--
string(9) "中国abc"
string(3) "abc"
string(6) "字符"
string(5) "china"
string(6) "string"
