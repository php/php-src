<?php

$t = 3;

function busy_wait($how_long)
{
	$until = time() + $how_long;

	while ($until > time());
}

