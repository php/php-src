--TEST--
Switch test 2
--FILE--
<?php 

for ($i=0; $i<=5; $i++)
{
  echo "i=$i\n";
  
  switch($i) {
    case 0:
      echo "In branch 0\n";
      break;
    case 1:
      echo "In branch 1\n";
      break;
    case 2:
      echo "In branch 2\n";
      break;
    case 3:
      echo "In branch 3\n";
      break 2;
    case 4:
      echo "In branch 4\n";
      break;
    default:
      echo "In default\n";
      break;
  }
}
echo "hi\n";
?>
--EXPECT--
i=0
In branch 0
i=1
In branch 1
i=2
In branch 2
i=3
In branch 3
hi
