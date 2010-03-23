--TEST--
substr_replace() function
--FILE--
<?php
$str = "try this";
$repl = "bala ";
$start = 2;
echo "\n";


echo "substr_replace('$str', '$repl', $start)\n";
var_dump(substr_replace($str, $repl, $start));
echo "\n";

$len = 3;
echo "substr_replace('$str', '$repl', $start, $len)\n";
var_dump(substr_replace($str, $repl, $start, $len));
echo "\n";

$len = 0;
echo "substr_replace('$str', '$repl', $start, $len)\n";
var_dump(substr_replace($str, $repl, $start, $len));
echo "\n";

$len = -2;
echo "substr_replace('$str', '$repl', $start, $len)\n";
var_dump(substr_replace($str, $repl, $start, $len));
echo "\n";
echo "\n";
echo "\n";


$str = "try this";
$repl = array("bala ");
$start = 4;
$len = 3;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).", ".var_export($start,1)."")."\n";
var_dump(substr_replace($str, $repl, $start))."\n";
echo "\n";
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).", ".var_export($start,1)."")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";

echo "\n";
echo "\n";
echo "\n";



$str = array("ala portokala");
$repl = array("bala ");
$start = array(4);
$len = array(3);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).", ".var_export($start,1)."")."\n";
var_dump(substr_replace($str, $repl, $start))."\n";
echo "\n";

$len = array(3);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).", ".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";

$len = array(0);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).", ".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";

$len = array(-2);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).", ".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";
echo "\n";




$str = array("ala portokala");
$repl = "bala ";
$start = 4;
$len = 3;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).")")."\n";
var_dump(substr_replace($str, $repl, $start))."\n";
echo "\n";
echo "\n";



$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = 4;
$len = 3;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = 4;
$len = 3;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = 4;
$len = 0;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = 4;
$len = 0;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";

$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = 4;
$len = -2;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = 4;
$len = -2;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";
echo "\n";
echo "\n";
echo "\n";
echo "\n";




$str = array("ala portokala");
$repl = "bala ";
$start = array(4);
$len = 3;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).")")."\n";
var_dump(substr_replace($str, $repl, $start))."\n";
echo "\n";
echo "\n";



$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = array(4);
$len = 3;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = array(4);
$len = 3;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = array(4);
$len = 0;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = array(4);
$len = 0;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";

$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = array(4);
$len = -2;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = array(4);
$len = -2;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";


echo "\n";
echo "\n";
echo "\n";
echo "\n";
echo "\n";
echo "\n";


$str = array("ala portokala");
$repl = "bala ";
$start = array(4,2);
$len = 3;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).")")."\n";
var_dump(substr_replace($str, $repl, $start))."\n";
echo "\n";
echo "\n";



$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = array(4,2);
$len = 3;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = array(4,2);
$len = 3;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = array(4,2);
$len = 0;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = array(4,2);
$len = 0;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";

$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = array(4,2);
$len = -2;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = array(4,2);
$len = -2;
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";



echo "\n";
echo "\n";
echo "\n";
echo "\n";
echo "\n";
echo "\n";


$str = array("ala portokala");
$repl = "bala ";
$start = array(4,2);
$len = array(3);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).")")."\n";
var_dump(substr_replace($str, $repl, $start))."\n";
echo "\n";
echo "\n";



$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = array(4,2);
$len = array(3);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = array(4,2);
$len = array(3);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = array(4,2);
$len = array(0);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = array(4,2);
$len = array(0);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";

$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = array(4,2);
$len = array(-2);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = array(4,2);
$len = array(-2);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";


echo "\n";
echo "\n";
echo "\n";
echo "\n";
echo "\n";
echo "\n";


$str = array("ala portokala");
$repl = "bala ";
$start = array(4,2);
$len = array(3,2);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).")")."\n";
var_dump(substr_replace($str, $repl, $start))."\n";
echo "\n";
echo "\n";



$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = array(4,2);
$len = array(3,2);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = array(4,2);
$len = array(3,2);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = array(4,2);
$len = array(0,0);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = array(4,2);
$len = array(0,0);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";

$str = array("ala portokala", "try this");
$repl = array("bala ");
$start = array(4,2);
$len = array(-2,-3);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";
echo "\n";


$str = array("ala portokala", "try this");
$repl = "bala ";
$start = array(4,2);
$len = array(-2,-3);
echo str_replace("\n","","substr_replace(".var_export($str,1).", ".var_export($repl,1).",".var_export($start,1).", ".var_export($len,1).")")."\n";
var_dump(substr_replace($str, $repl, $start, $len))."\n";
echo "\n";




?>
--EXPECT--
substr_replace('try this', 'bala ', 2)
string(7) "trbala "

substr_replace('try this', 'bala ', 2, 3)
string(10) "trbala his"

substr_replace('try this', 'bala ', 2, 0)
string(13) "trbala y this"

substr_replace('try this', 'bala ', 2, -2)
string(9) "trbala is"



substr_replace('try this', array (  0 => 'bala ',), 4
string(9) "try bala "

substr_replace('try this', array (  0 => 'bala ',), 4
string(10) "try bala s"




substr_replace(array (  0 => 'ala portokala',), array (  0 => 'bala ',), array (  0 => 4,)
array(1) {
  [0]=>
  string(9) "ala bala "
}

substr_replace(array (  0 => 'ala portokala',), array (  0 => 'bala ',), array (  0 => 4,), array (  0 => 3,))
array(1) {
  [0]=>
  string(15) "ala bala tokala"
}

substr_replace(array (  0 => 'ala portokala',), array (  0 => 'bala ',), array (  0 => 4,), array (  0 => 0,))
array(1) {
  [0]=>
  string(18) "ala bala portokala"
}

substr_replace(array (  0 => 'ala portokala',), array (  0 => 'bala ',), array (  0 => 4,), array (  0 => -2,))
array(1) {
  [0]=>
  string(11) "ala bala la"
}



substr_replace(array (  0 => 'ala portokala',), 'bala ',4)
array(1) {
  [0]=>
  string(9) "ala bala "
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),4, 3)
array(2) {
  [0]=>
  string(15) "ala bala tokala"
  [1]=>
  string(5) "try s"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',4, 3)
array(2) {
  [0]=>
  string(15) "ala bala tokala"
  [1]=>
  string(10) "try bala s"
}

substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),4, 0)
array(2) {
  [0]=>
  string(18) "ala bala portokala"
  [1]=>
  string(8) "try this"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',4, 0)
array(2) {
  [0]=>
  string(18) "ala bala portokala"
  [1]=>
  string(13) "try bala this"
}

substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),4, -2)
array(2) {
  [0]=>
  string(11) "ala bala la"
  [1]=>
  string(6) "try is"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',4, -2)
array(2) {
  [0]=>
  string(11) "ala bala la"
  [1]=>
  string(11) "try bala is"
}






substr_replace(array (  0 => 'ala portokala',), 'bala ',array (  0 => 4,))
array(1) {
  [0]=>
  string(9) "ala bala "
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),array (  0 => 4,), 3)
array(2) {
  [0]=>
  string(15) "ala bala tokala"
  [1]=>
  string(5) " this"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',array (  0 => 4,), 3)
array(2) {
  [0]=>
  string(15) "ala bala tokala"
  [1]=>
  string(10) "bala  this"
}

substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),array (  0 => 4,), 0)
array(2) {
  [0]=>
  string(18) "ala bala portokala"
  [1]=>
  string(8) "try this"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',array (  0 => 4,), 0)
array(2) {
  [0]=>
  string(18) "ala bala portokala"
  [1]=>
  string(13) "bala try this"
}

substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),array (  0 => 4,), -2)
array(2) {
  [0]=>
  string(11) "ala bala la"
  [1]=>
  string(2) "is"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',array (  0 => 4,), -2)
array(2) {
  [0]=>
  string(11) "ala bala la"
  [1]=>
  string(7) "bala is"
}







substr_replace(array (  0 => 'ala portokala',), 'bala ',array (  0 => 4,  1 => 2,))
array(1) {
  [0]=>
  string(9) "ala bala "
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),array (  0 => 4,  1 => 2,), 3)
array(2) {
  [0]=>
  string(15) "ala bala tokala"
  [1]=>
  string(5) "trhis"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',array (  0 => 4,  1 => 2,), 3)
array(2) {
  [0]=>
  string(15) "ala bala tokala"
  [1]=>
  string(10) "trbala his"
}

substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),array (  0 => 4,  1 => 2,), 0)
array(2) {
  [0]=>
  string(18) "ala bala portokala"
  [1]=>
  string(8) "try this"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',array (  0 => 4,  1 => 2,), 0)
array(2) {
  [0]=>
  string(18) "ala bala portokala"
  [1]=>
  string(13) "trbala y this"
}

substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),array (  0 => 4,  1 => 2,), -2)
array(2) {
  [0]=>
  string(11) "ala bala la"
  [1]=>
  string(4) "tris"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',array (  0 => 4,  1 => 2,), -2)
array(2) {
  [0]=>
  string(11) "ala bala la"
  [1]=>
  string(9) "trbala is"
}







substr_replace(array (  0 => 'ala portokala',), 'bala ',array (  0 => 4,  1 => 2,))
array(1) {
  [0]=>
  string(9) "ala bala "
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),array (  0 => 4,  1 => 2,), array (  0 => 3,))
array(2) {
  [0]=>
  string(15) "ala bala tokala"
  [1]=>
  string(2) "tr"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',array (  0 => 4,  1 => 2,), array (  0 => 3,))
array(2) {
  [0]=>
  string(15) "ala bala tokala"
  [1]=>
  string(7) "trbala "
}

substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),array (  0 => 4,  1 => 2,), array (  0 => 0,))
array(2) {
  [0]=>
  string(18) "ala bala portokala"
  [1]=>
  string(2) "tr"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',array (  0 => 4,  1 => 2,), array (  0 => 0,))
array(2) {
  [0]=>
  string(18) "ala bala portokala"
  [1]=>
  string(7) "trbala "
}

substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),array (  0 => 4,  1 => 2,), array (  0 => -2,))
array(2) {
  [0]=>
  string(11) "ala bala la"
  [1]=>
  string(2) "tr"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',array (  0 => 4,  1 => 2,), array (  0 => -2,))
array(2) {
  [0]=>
  string(11) "ala bala la"
  [1]=>
  string(7) "trbala "
}







substr_replace(array (  0 => 'ala portokala',), 'bala ',array (  0 => 4,  1 => 2,))
array(1) {
  [0]=>
  string(9) "ala bala "
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),array (  0 => 4,  1 => 2,), array (  0 => 3,  1 => 2,))
array(2) {
  [0]=>
  string(15) "ala bala tokala"
  [1]=>
  string(6) "trthis"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',array (  0 => 4,  1 => 2,), array (  0 => 3,  1 => 2,))
array(2) {
  [0]=>
  string(15) "ala bala tokala"
  [1]=>
  string(11) "trbala this"
}

substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),array (  0 => 4,  1 => 2,), array (  0 => 0,  1 => 0,))
array(2) {
  [0]=>
  string(18) "ala bala portokala"
  [1]=>
  string(8) "try this"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',array (  0 => 4,  1 => 2,), array (  0 => 0,  1 => 0,))
array(2) {
  [0]=>
  string(18) "ala bala portokala"
  [1]=>
  string(13) "trbala y this"
}

substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), array (  0 => 'bala ',),array (  0 => 4,  1 => 2,), array (  0 => -2,  1 => -3,))
array(2) {
  [0]=>
  string(11) "ala bala la"
  [1]=>
  string(5) "trhis"
}


substr_replace(array (  0 => 'ala portokala',  1 => 'try this',), 'bala ',array (  0 => 4,  1 => 2,), array (  0 => -2,  1 => -3,))
array(2) {
  [0]=>
  string(11) "ala bala la"
  [1]=>
  string(10) "trbala his"
}

