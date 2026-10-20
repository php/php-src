--TEST--
Testing mb_ereg_search() function
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
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
--EXPECTF--
Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(9) "中国abc"

Deprecated: Function mb_ereg_search_regs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(3) "abc"

Deprecated: Function mb_ereg_search_regs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "字符"

Deprecated: Function mb_ereg_search_regs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "china"

Deprecated: Function mb_ereg_search_regs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "string"

Deprecated: Function mb_ereg_search_regs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
