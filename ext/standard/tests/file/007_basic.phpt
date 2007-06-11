--TEST--
Test fopen(), fclose() & feof() functions: basic functionality
--FILE--
<?php
/* 
 Prototype: resource fopen(string $filename, string $mode 
                            [, bool $use_include_path [, resource $context]] );
 Description: Opens file or URL.

 Prototype: bool fclose ( resource $handle );
 Description: Closes an open file pointer

 Prototype: bool feof ( resource $handle );
 Description: Tests for end-of-file on a file pointer
*/

echo "*** Testing basic operations of fopen() and fclose() functions ***\n";
$modes = array(
  "w",
  "wb",
  "wt",
  "w+",
  "w+b",
  "w+t",

  "r",
  "rb",
  "rt",
  "r+",
  "r+b",
  "r+t",

  "a",
  "ab",
  "at",
  "a+",
  "a+t",
  "a+b"
);

for( $i=0; $i<count($modes); $i++ ) {
  echo "\n-- Iteration with mode '$modes[$i]' --\n";

  $filename = dirname(__FILE__)."/007_basic.tmp";
  // check fopen()
  $handle = fopen($filename, $modes[$i]);
  var_dump($handle );
  var_dump( ftell($handle) );
  var_dump( feof($handle) );

  // check fclose()
  var_dump( fclose($handle) );
  var_dump( $handle );
  // confirm the closure, using ftell() and feof(), expect, false
  var_dump( ftell($handle) );
  var_dump( feof($handle) );
}

// remove the temp file
unlink($filename);

$x_modes = array(
  "x",
  "xb",
  "xt",
  "x+",
  "x+b",
  "x+t"
);

for( $i=0; $i<count($x_modes); $i++ ) {
  echo "\n-- Iteration with mode '$x_modes[$i]' --\n";
  $handle = fopen($filename, $x_modes[$i]);
  var_dump($handle );
  var_dump( ftell($handle) );
  var_dump( feof($handle) );

  // check fclose()
  var_dump( fclose($handle) );
  var_dump( $handle );
  // confirm the closure, using ftell() and feof(), expect, false
  var_dump( ftell($handle) );
  var_dump( feof($handle) );
  var_dump( $handle );

  // remove the file
  unlink( $filename );
}

echo "\n*** Done ***\n";
--EXPECTF--
*** Testing basic operations of fopen() and fclose() functions ***

-- Iteration with mode 'w' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'wb' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'wt' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'w+' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'w+b' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'w+t' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'r' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'rb' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'rt' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'r+' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'r+b' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'r+t' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'a' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'ab' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'at' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'a+' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'a+t' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'a+b' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

-- Iteration with mode 'x' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)
resource(%d) of type (Unknown)

-- Iteration with mode 'xb' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)
resource(%d) of type (Unknown)

-- Iteration with mode 'xt' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)
resource(%d) of type (Unknown)

-- Iteration with mode 'x+' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)
resource(%d) of type (Unknown)

-- Iteration with mode 'x+b' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)
resource(%d) of type (Unknown)

-- Iteration with mode 'x+t' --
resource(%d) of type (stream)
int(0)
bool(false)
bool(true)
resource(%d) of type (Unknown)

Warning: ftell(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)
resource(%d) of type (Unknown)

*** Done ***
