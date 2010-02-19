--TEST--
Test Blowfish crypt() with invalid rounds
--XFAIL--
Needs a patch from Pierre
--FILE--
<?php

foreach(range(32, 38) as $i) {
  if (crypt('U*U', '$2a$'.$i.'$CCCCCCCCCCCCCCCCCCCCCC$') === FALSE) {
    echo "$i. OK\n";
  } else {
    echo "$i. Not OK\n";
  }
}

?>
--EXPECT--
32. OK
33. OK
34. OK
35. OK
36. OK
37. OK
38. OK
