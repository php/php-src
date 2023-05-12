--TEST--
Collection: Syntax
--FILE--
<?php
collection Articles(int => Article)
{
}

$c = new Articles;

var_dump($c);
