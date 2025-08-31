<?php

echo "hello";
sleep(1);
fwrite(STDERR, "SOME ERROR");
sleep(1);
echo "world";
