<?php

interface RecursiveIterator implements Iterator
{
	function hasChildren();
	function getChildren();
}

?>