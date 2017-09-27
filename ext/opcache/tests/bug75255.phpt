--TEST--
Bug #75255 (Request hangs and not finish)
--FILE--
<?php
function generatePlanImage() {
    if ($abc > 5) {
        $abc = 5;
    }
    for ($row = 0; $row < $abc; $row++) {
        for ($col = 0; $col < $numCols; $col++) {
            getPossibleRatio($abc);
        }
    }
}
?>
DONE
--EXPECT--
DONE
