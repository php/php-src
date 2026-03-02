<?php
class Reproducer
{
    /**
     * Remove $params arg and segfaults will go away
     */
    public static function loops(array $params = []): int
    {
        $arrCount = 2000;
        # Replace `$arrCount % 16` with 0 and segfaults will go away
        $arrCount2 = $arrCount - $arrCount % 16;
        $result = 0;

        for ($baseIdx = 0; $baseIdx < $arrCount2; $baseIdx++) {
        }

        while ($baseIdx < $arrCount) {
        }

        return $result;
    }
}
?>