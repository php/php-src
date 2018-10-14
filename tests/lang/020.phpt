--TEST--
Switch test 1
--FILE--
<?php

$i="abc";

for ($j=0; $j<10; $j++) {
switch (1) {
  case 1:
  	echo "In branch 1\n";
  	switch ($i) {
  		case "ab":
  			echo "This doesn't work... :(\n";
  			break;
  		case "abcd":
  			echo "This works!\n";
  			break;
  		case "blah":
  			echo "Hmmm, no worki\n";
  			break;
  		default:
  			echo "Inner default...\n";
  	}
  	for ($blah=0; $blah<200; $blah++) {
  	  if ($blah==100) {
  	    echo "blah=$blah\n";
  	  }
  	}
  	break;
  case 2:
  	echo "In branch 2\n";
  	break;
  case $i:
  	echo "In branch \$i\n";
  	break;
  case 4:
  	echo "In branch 4\n";
  	break;
  default:
  	echo "Hi, I'm default\n";
  	break;
 }
}
?>
--EXPECT--
In branch 1
Inner default...
blah=100
In branch 1
Inner default...
blah=100
In branch 1
Inner default...
blah=100
In branch 1
Inner default...
blah=100
In branch 1
Inner default...
blah=100
In branch 1
Inner default...
blah=100
In branch 1
Inner default...
blah=100
In branch 1
Inner default...
blah=100
In branch 1
Inner default...
blah=100
In branch 1
Inner default...
blah=100
