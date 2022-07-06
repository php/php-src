--TEST--
Bug #75608 ("Narrowing occurred during type inference" error)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class ReactionRatingService
{
    public function calculateBoostPoints()
    {
        while ($reaction = $reactions) {
            $reactionRatings = $this->validFunction();

            $totalWeight  = 0;
            $runningScore = 0;
            $queue        = [];
            foreach ($reactionRatings as $ratingData) {
                if ($runningScore != $reaction['Score']) {
                    if ( ! $ratingData['BoostEarned']) {
                        $queue[] = $ratingData['UserID'];
                    }
                } else {
                    foreach ($queue as $userId) {
                        $userBoostPointsRecalculate[$userId][] = $reaction['ID'];
                    }
                }
                $totalWeight += $ratingData['Weight'];
            }
        }
    }
}
?>
OK
--EXPECT--
OK
