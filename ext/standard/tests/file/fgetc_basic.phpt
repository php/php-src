--TEST--
Test fgetc() function : basic functionality
--FILE--
<?php
/*
 Prototype: string fgetc ( resource $handle );
 Description: Gets character from file pointer
*/
// include the header for common test function
include ("file.inc");

echo "*** Testing fgetc() : basic operations ***\n";
/* read charecter from different files which are opened in different modes */
$file_modes = array( "r", "rb", "rt", "r+", "r+b", "r+t");

/* create file with following type of contents */
$file_content_types = array("numeric", "text", "text_with_new_line");

for($outerloop_counter = 0; $outerloop_counter < count($file_content_types); $outerloop_counter++) {
  echo "--- Outerloop iteration ";
  echo $outerloop_counter + 1;
  echo " ---\n";
  // create file file
  create_files(__DIR__, 1, $file_content_types[$outerloop_counter], 0755, 1, "w", "fgetc_basic", 1);

  //open the file in different modes and check the working of fgetc
  for($innerloop_counter = 0; $innerloop_counter < count($file_modes); $innerloop_counter++) {
    echo "-- Innerloop iteration ";
    echo $innerloop_counter + 1;
    echo " of Outerloop Iteration ";
    echo $outerloop_counter + 1;
    echo " --\n";

    // open the file using the $file_modes
    $filename = __DIR__."/fgetc_basic1.tmp"; // file name that is created by create_files
    echo "-- Testing fgetc() : file opened using $file_modes[$innerloop_counter] mode --\n";
    $file_handle = fopen($filename, $file_modes[$innerloop_counter]);
    if ( !$file_handle ) {
      echo "Error: failed to open file $filename!";
      exit();
    }

    // perform the read file at least 6 char and check
    for( $counter = 1; $counter <= 6; $counter++ ) {
      // read data from the file and check, file pointer position, feof etc
      var_dump( fgetc($file_handle) ); // read a char
      var_dump( ftell($file_handle) ); // file pointer position
      var_dump( feof($file_handle) );  // is it eof()
      var_dump($file_handle); // dump the $file_handle to see if any thing got modifed
    } // end of for

    // close the file
    fclose ( $file_handle);

  } // end of innerloop for

  // delete the file
  delete_files(__DIR__, 1, "fgetc_basic", 1, ".tmp");

} // end of outerloop for

echo "Done\n";
?>
--EXPECTF--
*** Testing fgetc() : basic operations ***
--- Outerloop iteration 1 ---
-- Innerloop iteration 1 of Outerloop Iteration 1 --
-- Testing fgetc() : file opened using r mode --
string(1) "2"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 2 of Outerloop Iteration 1 --
-- Testing fgetc() : file opened using rb mode --
string(1) "2"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 3 of Outerloop Iteration 1 --
-- Testing fgetc() : file opened using rt mode --
string(1) "2"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 4 of Outerloop Iteration 1 --
-- Testing fgetc() : file opened using r+ mode --
string(1) "2"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 5 of Outerloop Iteration 1 --
-- Testing fgetc() : file opened using r+b mode --
string(1) "2"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 6 of Outerloop Iteration 1 --
-- Testing fgetc() : file opened using r+t mode --
string(1) "2"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "2"
int(6)
bool(false)
resource(%d) of type (stream)
--- Outerloop iteration 2 ---
-- Innerloop iteration 1 of Outerloop Iteration 2 --
-- Testing fgetc() : file opened using r mode --
string(1) "t"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "e"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "x"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "t"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) " "
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "t"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 2 of Outerloop Iteration 2 --
-- Testing fgetc() : file opened using rb mode --
string(1) "t"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "e"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "x"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "t"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) " "
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "t"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 3 of Outerloop Iteration 2 --
-- Testing fgetc() : file opened using rt mode --
string(1) "t"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "e"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "x"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "t"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) " "
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "t"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 4 of Outerloop Iteration 2 --
-- Testing fgetc() : file opened using r+ mode --
string(1) "t"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "e"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "x"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "t"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) " "
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "t"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 5 of Outerloop Iteration 2 --
-- Testing fgetc() : file opened using r+b mode --
string(1) "t"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "e"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "x"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "t"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) " "
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "t"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 6 of Outerloop Iteration 2 --
-- Testing fgetc() : file opened using r+t mode --
string(1) "t"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "e"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "x"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "t"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) " "
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "t"
int(6)
bool(false)
resource(%d) of type (stream)
--- Outerloop iteration 3 ---
-- Innerloop iteration 1 of Outerloop Iteration 3 --
-- Testing fgetc() : file opened using r mode --
string(1) "l"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "i"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "n"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "e"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) "
"
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "l"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 2 of Outerloop Iteration 3 --
-- Testing fgetc() : file opened using rb mode --
string(1) "l"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "i"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "n"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "e"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) "
"
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "l"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 3 of Outerloop Iteration 3 --
-- Testing fgetc() : file opened using rt mode --
string(1) "l"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "i"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "n"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "e"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) "
"
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "l"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 4 of Outerloop Iteration 3 --
-- Testing fgetc() : file opened using r+ mode --
string(1) "l"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "i"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "n"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "e"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) "
"
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "l"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 5 of Outerloop Iteration 3 --
-- Testing fgetc() : file opened using r+b mode --
string(1) "l"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "i"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "n"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "e"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) "
"
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "l"
int(6)
bool(false)
resource(%d) of type (stream)
-- Innerloop iteration 6 of Outerloop Iteration 3 --
-- Testing fgetc() : file opened using r+t mode --
string(1) "l"
int(1)
bool(false)
resource(%d) of type (stream)
string(1) "i"
int(2)
bool(false)
resource(%d) of type (stream)
string(1) "n"
int(3)
bool(false)
resource(%d) of type (stream)
string(1) "e"
int(4)
bool(false)
resource(%d) of type (stream)
string(1) "
"
int(5)
bool(false)
resource(%d) of type (stream)
string(1) "l"
int(6)
bool(false)
resource(%d) of type (stream)
Done
