--TEST--
Test inserting data into a DOMComment basic test
--CREDITS--
Andrew Larssen <al@larssen.org>
London TestFest 2008
--EXTENSIONS--
dom
--FILE--
<?php

//Negative offset
$dom = new DomDocument();
$comment = $dom->createComment('test-comment');
try {
  $comment->insertData(-1,'-inserted');
} catch (DOMException $e ) {
  if ($e->getMessage() == 'Index Size Error'){
    echo "Throws DOMException for -ve offset\n";
  }
}

?>
--EXPECT--
Throws DOMException for -ve offset
