--TEST--
#-style comments
--FILE--
#teste
#teste2
<?php

#ahahah
#ahhfhf

echo '#ola'; //?
echo "\n";
echo 'uhm # ah'; #ah?
echo "\n";
echo "e este, # hein?";
echo "\n";

?>
--EXPECT--
#teste
#teste2
#ola
uhm # ah
e este, # hein?
