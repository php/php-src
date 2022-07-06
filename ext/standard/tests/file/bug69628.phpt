--TEST--
Bug #69628: GLOB_BRACE with multiple brackets within the braces fails
--SKIPIF--
<?php
if (!defined('GLOB_BRACE')) {
    die('skip this test requires GLOB_BRACE support');
}
?>
--FILE--
<?php

$file_path = __DIR__;

// temp dirname used here
$dirname = "$file_path/bug69628";

// temp dir created
mkdir($dirname);

// temp files created
file_put_contents("$dirname/image.jPg", '');
file_put_contents("$dirname/image.gIf", '');
file_put_contents("$dirname/image.png", '');

sort_var_dump(glob("$dirname/*.{[jJ][pP][gG],[gG][iI][fF]}", GLOB_BRACE));

function sort_var_dump($results) {
   sort($results);
   var_dump($results);
}

?>
--CLEAN--
<?php

$file_path = __DIR__;
unlink("$file_path/bug69628/image.jPg");
unlink("$file_path/bug69628/image.gIf");
unlink("$file_path/bug69628/image.png");
rmdir("$file_path/bug69628/");

?>
--EXPECTF--
array(2) {
  [0]=>
  string(%d) "%s/bug69628/image.gIf"
  [1]=>
  string(%d) "%s/bug69628/image.jPg"
}
