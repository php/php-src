<?php

echo "hello";
// Wait until the parent has read the first chunk.
fgets(STDIN);
echo "world";

echo strtoupper(trim(fgets(STDIN)));
