--TEST--
Sybase-CT sybase_fetch_object
--SKIPIF--
<?php 
  require('skipif.inc');
?>
--FILE--
<?php
/* This file is part of PHP test framework for ext/sybase_ct
 *
 * $Id$
 */

  require('test.inc');
  
  // {{{ class article
  class article { }
  // }}}
  
  // {{{ resource fetch_object(resource db, [mixed arg= NULL])
  //     Fetches a resultset and returns it as an object
  function fetch_object($db, $arg= NULL) {
    return sybase_fetch_object(sybase_query('select
      1 as "id",
      "Hello" as "caption",
      "timm" as "author",
      getdate() as "lastchange"
    ', $db), $arg);
  }
  // }}}

  $db= sybase_connect_ex();
  
  // Test with stdClass
  var_export(fetch_object($db)); echo "\n";
  
  // Test with userland class
  var_export(fetch_object($db, 'article')); echo "\n";

  // Test with object
  var_export(fetch_object($db, new article())); echo "\n";

  // Test with non-existent class
  var_export(fetch_object($db, '***')); echo "\n";
    
  sybase_close($db);
?>
--EXPECTF--
stdClass::__set_state(array(
   'id' => 1,
   'caption' => 'Hello',
   'author' => 'timm',
   'lastchange' => '%s',
))
article::__set_state(array(
   'id' => 1,
   'caption' => 'Hello',
   'author' => 'timm',
   'lastchange' => '%s',
))
article::__set_state(array(
   'id' => 1,
   'caption' => 'Hello',
   'author' => 'timm',
   'lastchange' => '%s',
))

Notice: sybase_fetch_object(): Sybase:  Class *** has not been declared in %stest_fetch_object.php on line %d
stdClass::__set_state(array(
   'id' => 1,
   'caption' => 'Hello',
   'author' => 'timm',
   'lastchange' => '%s',
))
