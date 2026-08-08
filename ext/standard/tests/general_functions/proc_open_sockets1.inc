<?php

echo "hello";
fgets(STDIN);
fwrite(STDERR, "SOME ERROR");
fgets(STDIN);
echo "world";
fgets(STDIN);
