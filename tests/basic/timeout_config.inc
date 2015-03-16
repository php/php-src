<?php

$t = 3;

function busy_sleep($how_long)
{
	$now = time();

	while($now + $how_long > time());
}

