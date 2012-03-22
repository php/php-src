--TEST--
Testing mb_eregi_replace() function
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring not enabled');
function_exists('mb_eregi_replace') or die("skip mb_eregi_replace() is not available in this build");
?>
--FILE--
<?php
function do_translit($st) {
    $replacement = array(
        "й"=>"i","ц"=>"c","у"=>"u","к"=>"k","е"=>"e","н"=>"n",
        "г"=>"g","ш"=>"sh","щ"=>"sh","з"=>"z","х"=>"x","ъ"=>"\'",
        "ф"=>"f","ы"=>"i","в"=>"v","а"=>"a","п"=>"p","р"=>"r",
        "о"=>"o","л"=>"l","д"=>"d","ж"=>"zh","э"=>"ie","ё"=>"e",
        "я"=>"ya","ч"=>"ch","с"=>"c","м"=>"m","и"=>"i","т"=>"t",
        "ь"=>"\'","б"=>"b","ю"=>"yu",
        "Й"=>"I","Ц"=>"C","У"=>"U","К"=>"K","Е"=>"E","Н"=>"N",
        "Г"=>"G","Ш"=>"SH","Щ"=>"SH","З"=>"Z","Х"=>"X","Ъ"=>"\'",
        "Ф"=>"F","Ы"=>"I","В"=>"V","А"=>"A","П"=>"P","Р"=>"R",
        "О"=>"O","Л"=>"L","Д"=>"D","Ж"=>"ZH","Э"=>"IE","Ё"=>"E",
        "Я"=>"YA","Ч"=>"CH","С"=>"C","М"=>"M","И"=>"I","Т"=>"T",
        "Ь"=>"\'","Б"=>"B","Ю"=>"YU",
    );
   
    foreach($replacement as $i=>$u) {
        $st = mb_eregi_replace($i,$u,$st);
    }
    return $st;
} 

echo do_translit("Пеар");
?>
--EXPECT--
Pear
--CREDITS--
Testfest Wuerzburg 2009-06-20 (modified by rui 2011-10-15)
