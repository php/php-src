--TEST--
Test realpath() function : variation
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == 'WIN' )
  die("skip Not Valid for Windows");
?>
--FILE--
<?php
echo "*** Testing realpath() : variation ***\n";

$paths = array('c:\\',
               'c:',
               'c' ,
               '\\' ,
               '/',
               'c:temp',
               'c:\\/',
               '/tmp/',
               '/tmp/\\',
               '\\tmp',
               '\\tmp\\');

foreach($paths as $path) {
      echo "\n--$path--\n";
      var_dump( realpath($path) );
};

?>
--EXPECTF--
*** Testing realpath() : variation ***

--c:\--
bool(false)

--c:--
bool(false)

--c--
bool(false)

--\--
bool(false)

--/--
string(1) "/"

--c:temp--
bool(false)

--c:\/--
bool(false)

--/tmp/--
string(%d) %s/tmp"

--/tmp/\--
bool(false)

--\tmp--
bool(false)

--\tmp\--
bool(false)
