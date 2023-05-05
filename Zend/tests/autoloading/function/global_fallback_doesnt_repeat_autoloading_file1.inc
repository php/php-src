<?php
namespace bar;

echo "First file START\n";

foo();

for ($i = 0; $i < 3; $i += 1) {
    foo();
}

echo "First file END\n";
