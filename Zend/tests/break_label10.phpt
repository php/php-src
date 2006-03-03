--TEST--
labeled break 10: foreach
--FILE--
<?php

Lend:
foreach(array(1,2,3) as $k1 => $v1)
{
	echo "$k1=>$v1\n";
	foreach(array(1,2,3) as $k2 => $v2)
	{
		echo "$k2=>$v2\n";
		if ($v2 == 2)
		{
			if ($v1==2)
			{
				break Lend;
			}
			break;
		}
	}
}

?>
===DONE===
--EXPECTF--
0=>1
0=>1
1=>2
1=>2
0=>1
1=>2
===DONE===
