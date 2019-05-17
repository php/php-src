--TEST--
SPL: RegexIterator::getPregFlags()
--CREDITS--
Lance Kesson jac_kesson@hotmail.com
#testfest London 2009-05-09
--FILE--
<?php

class myIterator implements Iterator {

function current (){}
function key ( ){}
function next ( ){}
function rewind ( ){}
function valid ( ){}


}

class TestRegexIterator extends RegexIterator{}

$rege = '/^a/';


$r = new TestRegexIterator(new myIterator, $rege);

$r->setPregFlags(PREG_OFFSET_CAPTURE);

echo is_long($r->getPregFlags());

?>
--EXPECT--
1
