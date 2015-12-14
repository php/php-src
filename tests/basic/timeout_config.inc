<?php

$t = 3;

function busy_wait($how_long)
{
	$until = microtime(TRUE) + $how_long;

	while ($until > microtime(TRUE));
}

