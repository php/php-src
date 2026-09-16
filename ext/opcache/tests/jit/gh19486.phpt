--TEST--
GH-19486: incorrect opline after deoptimization
--INI--
opcache.jit_blacklist_root_trace=1
opcache.jit_blacklist_side_trace=1
--FILE--
<?php

(new GameMap())->getLakeArea(0, 0);

class GameMap
{
    public $lakeID = [];

    public function getLakeArea(int $x, int $y): int
    {
        $this->floodFill(0, 0, 0);
    }

    public function floodFill(int $x, int $y, int $id): void
    {
        if (($x < 0) or ($x >= 50) or ($y < 0) or ($y >= 50)) {
            return;
        }
        if (isset($this->lakeID[$y][$x]) and ($this->lakeID[$y][$x] == $id)) {
            return;
        }
        $this->lakeID[$y][$x] = $id;
        $this->floodFill($x - 1, $y, $id);
        $this->floodFill($x + 1, $y, $id);
        $this->floodFill($x, $y - 1, $id);
        $this->floodFill($x, $y + 1, $id);
    }
}

?>
--EXPECTF--
Fatal error: Uncaught TypeError: GameMap::getLakeArea(): Return value must be of type int, none returned in %s:%d
Stack trace:
#0 %s(%d): GameMap->getLakeArea(0, 0)
#1 {main}
  thrown in %s on line %d
