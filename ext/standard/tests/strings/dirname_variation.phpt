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
string\(11\) "~(\\|\/)home(\\|\/)user"

--Iteration 2 --
string\(11\) "~(\\|\/)home(\\|\/)user"

--Iteration 3 --
string\(11\) "~(\\|\/)home(\\|\/)user"

--Iteration 4 --
string\(11\) "~(\\|\/)home(\\|\/)user"

--Iteration 5 --
string\(19\) "hostname:(\\|\/)home(\\|\/)user"

--Iteration 6 --
string\(19\) "hostname:(\\|\/)home(\\|\/)user"

--Iteration 7 --
string\(19\) "hostname:(\\|\/)home(\\|\/)user"

--Iteration 8 --
string\(19\) "hostname:(\\|\/)home(\\|\/)user"

--Iteration 9 --
string\(19\) "hostname:(\\|\/)home(\\|\/)user"

--Iteration 10 --
string\(19\) "hostname:(\\|\/)home(\\|\/)user"

--Iteration 11 --
string\(1\) "."

--Iteration 12 --
string\(1\) "(\\|\/)"

--Iteration 13 --
string\(1\) "(\\|\/)"

--Iteration 14 --
string\(1\) "."

--Iteration 15 --
string\(2\) "10"

--Iteration 16 --
string\(1\) "."

--Iteration 17 --
string\(1\) "."

--Iteration 18 --
string\(1\) "."

--Iteration 19 --
string\(1\) "."

--Iteration 20 --
string\(1\) "."

--Iteration 21 --
string\(0\) ""

--Iteration 22 --
string\(0\) ""

--Iteration 23 --
string\(0\) ""

--Iteration 24 --
string\(0\) ""
Done
