--TEST--
Test dirname() function : usage variations
--FILE--
<?php
/* Prototype: string dirname ( string $path );
   Description: Returns directory name component of path.
*/
class temp
{ 
   function __toString() {
     return "Object";
   }
}

$file_path_variations = array (
  /* home dir shortcut char */
  "~/home/user/bar",
  "~/home/user/bar/",
  "~/home/user/bar.tar",
  "~/home/user/bar.tar/",

  /* with hotname:dir notation */
  "hostname:/home/user/bar.tar",
  "hostname:/home/user/tbar.gz/",
  "hostname:/home/user/My Pics.gz",
  "hostname:/home/user/My Pics.gz/",
  "hostname:/home/user/My Pics/",
  "hostname:/home/user/My Pics",
  
  /* path containing numeric string */
  "10.5",
  "/10.5",
  "/10.5/",
  "10.5/",
  "10/10.gz",
  '0',
  "0",
  
  /* object */
  new temp,

  /* path as spaces */
  " ",
  ' ',

  /* empty path */
  "",
  '',
  NULL,
  null
);

function check_dirname( $paths ) {
   $loop_counter = 0;
   $noOfPaths = count($paths);
   for( ; $loop_counter < $noOfPaths; $loop_counter++ ) {
     echo "\n--Iteration ";
     echo $loop_counter +1;
     echo " --\n"; 
     var_dump( dirname($paths[$loop_counter]) );
   }
}

echo "*** Testing possible variations in path ***\n";
check_dirname( $file_path_variations );

echo "Done\n";
?>
--EXPECTREGEX--
\*\*\* Testing possible variations in path \*\*\*

--Iteration 1 --
unicode\(11\) "~(\\|\/)home(\\|\/)user"

--Iteration 2 --
unicode\(11\) "~(\\|\/)home(\\|\/)user"

--Iteration 3 --
unicode\(11\) "~(\\|\/)home(\\|\/)user"

--Iteration 4 --
unicode\(11\) "~(\\|\/)home(\\|\/)user"

--Iteration 5 --
unicode\(19\) "hostname:(\\|\/)home(\\|\/)user"

--Iteration 6 --
unicode\(19\) "hostname:(\\|\/)home(\\|\/)user"

--Iteration 7 --
unicode\(19\) "hostname:(\\|\/)home(\\|\/)user"

--Iteration 8 --
unicode\(19\) "hostname:(\\|\/)home(\\|\/)user"

--Iteration 9 --
unicode\(19\) "hostname:(\\|\/)home(\\|\/)user"

--Iteration 10 --
unicode\(19\) "hostname:(\\|\/)home(\\|\/)user"

--Iteration 11 --
unicode\(1\) "."

--Iteration 12 --
unicode\(1\) "(\\|\/)"

--Iteration 13 --
unicode\(1\) "(\\|\/)"

--Iteration 14 --
unicode\(1\) "."

--Iteration 15 --
unicode\(2\) "10"

--Iteration 16 --
unicode\(1\) "."

--Iteration 17 --
unicode\(1\) "."

--Iteration 18 --
unicode\(1\) "."

--Iteration 19 --
unicode\(1\) "."

--Iteration 20 --
unicode\(1\) "."

--Iteration 21 --
unicode\(0\) ""

--Iteration 22 --
unicode\(0\) ""

--Iteration 23 --
unicode\(0\) ""

--Iteration 24 --
unicode\(0\) ""
Done
